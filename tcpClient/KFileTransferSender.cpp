//#include "stdafx.h"
#include "KFileTransferSender.h"

#include <QtConcurrent/QtConcurrent>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

KFileTransferSender::KFileTransferSender(QObject *parent)
    :QObject(parent)
    , _bCancel(false)
    , _nextBlockSize(0)
    , _recvDataSize(0)
{
    _pCommandSocket = new QTcpSocket(this);
    _pool.setMaxThreadCount(1);
}

void KFileTransferSender::connect_to_server(const QString &ipAddress, int port)
{
    _pCommandSocket->connectToHost(QHostAddress(ipAddress), port);
    connect(_pCommandSocket,SIGNAL(readyRead()),this,SLOT(on_read_command()));
}

void KFileTransferSender::sendFile(const QString &filePath, RetCallBack cbFun)
{
    if( set_file(filePath))
    {
        _cbSendFile = cbFun;
        send_command(FILE_HEAD_CODE);
    }
}

void KFileTransferSender::unSendFile(RetCallBack cbFun)
{
    _cbUnSendFile = cbFun;
    send_command(FILE_CANCEL);
}

void KFileTransferSender::isDiskFreeSpace(quint64 reqSpaceSize, RetCallBack cbFun)
{
    _cbIsFullSpaceWithFile = cbFun;
    send_command(FILE_IS_DISK_FREE_SPACE_CODE, reqSpaceSize);
}

void KFileTransferSender::isExistFile(const QString &filePath, RetCallBack cbFun)
{
    if(filePath.isEmpty())
    {
        qDebug()<< ERROR_CODE_1;
        return;
    }
    else
    {
        QFileInfo info(filePath);
        QString fileName = info.absoluteFilePath();
        qint64 fileSize = info.size();

        _cbIsExistFile = cbFun;
        send_command(FILE_IS_EXIST_CODE, fileName + SPLIT_ADDITION_INFO_MARK + QString::number(fileSize));
    }
}

bool KFileTransferSender::set_file(const QString &filePath)
{
    if(filePath.isEmpty())
    {
        qDebug()<< ERROR_CODE_1;
        return false;
    }
    QFileInfo info(filePath);
    _filename = info.fileName();
    _filesize = info.size();
    _sendSize = 0;

    if(_file.isOpen())
    {
        _file.close();
    }
    _file.setFileName(filePath);
    bool isOk = _file.open(QIODevice::ReadOnly);
    if(false == isOk)
    {
        qDebug()<<ERROR_CODE_2;
        return false;
    }
    return true;
}

bool KFileTransferSender::send_command(int type, QVariant additionalInfo)
{
    QString command;
    switch (type)
    {
        case FILE_HEAD_CODE:
        {
            command = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg(type).arg(_filename + SPLIT_ADDITION_INFO_MARK + QString::number(_filesize));
            break;
        }
        case FILE_CODE:
        {
            command = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg(type).arg("");
            break;
        }
        case FILE_CANCEL:
        {
            command = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg(type).arg("");
            break;
        }
        case FILE_IS_EXIST_CODE:
        {
            QString filePath = additionalInfo.toString();
            command = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg(type).arg(filePath);
            break;
        }
        case FILE_IS_DISK_FREE_SPACE_CODE:
        {
            quint64 fileSize = additionalInfo.toLongLong();
            command = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg(type).arg(QString::number(fileSize));
            break;
        }
        default:
            break;
    }

    qint64 len = _pCommandSocket->write(command.toUtf8());
    _pCommandSocket->waitForBytesWritten(-1);
    if(len <= 0)
    {
        qDebug()<< "命令代号:" << type << ERROR_CODE_3;
        _file.close();
        return false;
    }
    return true;
}

void KFileTransferSender::send_file()
{
    qint64 len = 0;
    QTcpSocket file_socket;
    file_socket.connectToHost(QHostAddress(IP), PORT_FILE);


    if (! _file.isOpen())
    {
        bool bOk = _file.open(QIODevice::ReadOnly);
        if (bOk)
        {
            qDebug() << "开始发送文件数据信息 " << QDateTime::currentDateTime().toString("YYYY-MM-DD HH:mm:ss:zzz");
        }
        else
        {
            qDebug()<<ERROR_CODE_2;
        }
    }

    do
    {
        QByteArray  buf;
        buf = _file.read(SEND_BLOCK_SIZE);
        len = file_socket.write(buf);
        file_socket.waitForBytesWritten(-1);
        _sendSize += len;

        _cancelFileTransferMutex.lock();
        if (_bCancel)
        {
            _cancelFileTransferMutex.unlock();
            break;
        }
        _cancelFileTransferMutex.unlock();
        qDebug() << "====>发送的数据数据大小:" << len / 1024 << "KB"  << "\t已发送数据大小:" << _sendSize / 1024 << "KB";
    } while(len > 0);

    if(_sendSize == _filesize)
    {
        _sendSize = 0;
        _file.close();
        file_socket.disconnect();
        file_socket.close();

        qDebug() << "文件 " << _file.fileName() << "发送完毕 "
                 << QDateTime::currentDateTime().toString("YYYY-MM-DD HH:mm:ss:zzz");
    }
    _cancelFileTransferMutex.lock();
    _bCancel = false;
    _cancelFileTransferMutex.unlock();
}

void KFileTransferSender::on_read_command()
{
    QDataStream out(_pCommandSocket);
    out.setVersion(QDataStream::Qt_5_9);
    if (_nextBlockSize == 0)
    {
        if (_pCommandSocket->bytesAvailable() >= sizeof(qint64))
        {
            out >> _nextBlockSize;
        }
    }

    if (_pCommandSocket->bytesAvailable() >= _nextBlockSize)
    {
        out >> _cacheData;
        dealReadCommand();
        _cacheData.clear();
        _nextBlockSize = 0;
    }
}

void KFileTransferSender::dealReadCommand()
{
    int code = _cacheData.section(SPLIT_TYPE_INFO_MAEK,0,0).toInt();
    int ret = _cacheData.section(SPLIT_TYPE_INFO_MAEK,1,1).toInt();
    QString additionalInfo = _cacheData.section(SPLIT_TYPE_INFO_MAEK,2,2);
    switch(code)
    {
    //发送文件头响应处理
    case FILE_HEAD_REC_CODE:
    {
#ifdef USE_THREAD_TRANSFER
        if(ret)
        {
            _fileTransferFuture = QtConcurrent::run(&_pool, [this] { send_file(); });
        }
        else
        {
            qDebug() << ERROR_CODE_4;
        }
#else
        ret ? send_file() : qDebug() << ERROR_CODE_4;
#endif
        break;
    }
    //接收文件数据完成响应处理
    case FILE_REC_CODE:
    {
        if(ret)
        {
            qint64 recv = additionalInfo.toLongLong();
            int progressVal = (recv * 100) / _filesize;
            emit progressValue(progressVal);
            qDebug() << "文件大小:" << _filesize / 1024 << "KB,\t已接收数据大小:" << recv / 1024 << "KB";
        }
        else
        {
            qDebug() << ERROR_CODE_5;
        }
        break;
    }
    //取消文件传输响应处理
    case FILE_REC_CANCEL:
    {
        _cancelFileTransferMutex.lock();
        _bCancel = true;
        _cancelFileTransferMutex.unlock();
        _sendSize = 0;
        if (_file.isOpen())
        {
            _file.close();
        }
        emit progressValue(0);
        if (_cbUnSendFile)
        {
            _cbUnSendFile(ret, additionalInfo);
        }
        break;
    }
    //检查查询的文件是否存在响应处理
    case FILE_IS_EXIST_REC_CODE:
    {
        if (_cbIsExistFile)
        {
            _cbIsExistFile(ret, additionalInfo);
        }
        break;
    }
    //判断添加文件后是否会空间不足响应处理
    case FILE_IS_DISK_FREE_SPACE_REC_CODE:
    {
        if (_cbIsFullSpaceWithFile)
        {
            _cbIsFullSpaceWithFile(ret, additionalInfo);
        }
        break;
    }
    default:
        qDebug() << __FUNCTION__ << ":read command error.";
        break;
    }
}
