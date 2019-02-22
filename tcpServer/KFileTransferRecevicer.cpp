//#include "stdafx.h"
#include "KFileTransferRecevicer.h"
#include "kfiletransfercachemanage.h"

#include<QtConcurrent/QtConcurrent>

KFileTransferRecevicer::KFileTransferRecevicer(QObject *parent)
    : QObject(parent)
    , _bCancel(false)
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/mediaCache/";
    setCacheDir(cacheDir);
    _pCommandSocket = new QTcpSocket(this);
    _pFileSocket = new QTcpSocket(this);
    _pTcpServerControl = new QTcpServer(this);
    _pTcpServerFile = new QTcpServer(this);
    _pFileSocket->setReadBufferSize(64*1024*1024);

    _pTcpServerControl->listen(QHostAddress::Any,PORT_COMMAND);
    _pTcpServerFile->listen(QHostAddress::Any,PORT_FILE);

    connect(_pTcpServerControl,SIGNAL(newConnection()),this,SLOT(on_connect_c()));
    connect(_pTcpServerFile,SIGNAL(newConnection()),this,SLOT(on_connect_f()));
}

bool KFileTransferRecevicer::isExistFileInCacheDir(const QString &fileName, qint64 fileSize)
{
    QFileInfo info(_fileCacheDir + "/" + fileName);
    qDebug() << __FUNCTION__ << info.absoluteFilePath();
    return (info.exists() && (info.size() == fileSize));
}

void KFileTransferRecevicer::setCacheDir(const QString &dir)
{
    QDir mdir(dir);
    if (! mdir.exists())
    {
        mdir.mkdir(dir);
        qDebug() << __FUNCTION__ << "缓存目录创建成功";
    }
    _fileCacheDir = dir;
}

void KFileTransferRecevicer::on_connect_c(){
    _pCommandSocket = _pTcpServerControl->nextPendingConnection();

    QString ip = _pCommandSocket->peerAddress().toString();
    quint16 port = _pCommandSocket->peerPort();

    qDebug()<<QString("[%1:%2]成功连接").arg(ip).arg(port);
    _pCommandSocket->disconnect();
    connect(_pCommandSocket,SIGNAL(readyRead()),this,SLOT(on_read_command()));
    connect(_pCommandSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onCommandError(QAbstractSocket::SocketError)));
}

void KFileTransferRecevicer::on_connect_f(){
    _pFileSocket = _pTcpServerFile->nextPendingConnection();

    QString ip = _pFileSocket->peerAddress().toString();
    quint16 port = _pFileSocket->peerPort();

    qDebug()<<QString("[%1:%2]成功连接").arg(ip).arg(port);
    _pFileSocket->disconnect();
    connect(_pFileSocket,SIGNAL(readyRead()),this,SLOT(on_read_file()));
    connect(_pFileSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onFileError(QAbstractSocket::SocketError)));
    _startTimeFlag = true;
}

void KFileTransferRecevicer::send_command(int code, int ret, QString additional)
{
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_9);
    QString head = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2" + SPLIT_TYPE_INFO_MAEK +"%3").arg(code).arg(ret).arg(additional);
    //QString head = QString("%1##%2##%3").arg(code).arg(ret).arg(additional);
    in << qint64(2) << head;
    in.device()->seek(0);
    in << qint64(data.size() - sizeof(qint64));
    qint64 len = _pCommandSocket->write(data);
    _pCommandSocket->waitForBytesWritten(-1);
    if(len <= 0)
    {
        qDebug()<<"命令发送失败 "<<code;
    }
    else
    {
        qDebug() << "发送命令:" << code << "执行结果:" << ret << "附加信息:" << additional;
    }
}

void KFileTransferRecevicer::on_read_command()
{
    QByteArray buf = _pCommandSocket->readAll();
    int code = QString(buf).section(SPLIT_TYPE_INFO_MAEK,0,0).toInt();
    QString additionalInfo = QString(buf).section(SPLIT_TYPE_INFO_MAEK,1,1);
    switch (code)
    {
    case FILE_HEAD_CODE:
    {
        if (additionalInfo.isEmpty()) { qDebug() << ERROR_CODE_4; return; }
        _fileName = additionalInfo.section(SPLIT_ADDITION_INFO_MARK, 0, 0);
        _fileSize = additionalInfo.section(SPLIT_ADDITION_INFO_MARK, 1, 1).toLongLong();
        _recvSize = 0;
        _bCancel = false;

        /////////////////////////////////////////////////
        //TODO: 这里判断
        //1.磁盘的大小是否足够，
        //2.文件是否已经存在等信息

        QDir::setCurrent(_fileCacheDir);
        _file.setFileName(_fileName);
        bool isOk = _file.open(QIODevice::WriteOnly);
        if(false == isOk)
        {
            qDebug()<< __FUNCTION__ << __LINE__ << "writeonly error.";
            _pCommandSocket->disconnectFromHost();
            _pCommandSocket->close();
            return;
        }

        qDebug()<<QString("文件名：%1\n大小:%2 kb").arg(_fileName).arg(_fileSize/1024);
        send_command(FILE_HEAD_REC_CODE, K_SUCCEED, SUCCEED_CODE_1);
        break;
    }
    case FILE_CODE:
    {
        //TODO: 开始接收文件数据信息;
        qDebug() << "开始接收文件数据信息 " << QDateTime::currentDateTime().toString("YYYY-MM-DD HH:mm:ss:zzz");
        break;
    }
    case FILE_CANCEL:
    {
        //TODO: 取消文件当前传输文件时需要做的操作;
        if(! _bCancel)
        {
            if(_file.isOpen())
            {
                QDir::setCurrent(_fileCacheDir);
                _file.close();
                _file.remove();
            }
            _bCancel = true;
            send_command(FILE_REC_CANCEL, K_SUCCEED, SUCCEED_CODE_3);
        }
        else
        {
            send_command(FILE_REC_CANCEL, K_ERROR, ERROR_CODE_7);
        }
        break;
    }
    case FILE_IS_EXIST_CODE:
    {
        if (additionalInfo.isEmpty()) { qDebug() << ERROR_CODE_8; return; }
        QString fileName = additionalInfo.section(SPLIT_ADDITION_INFO_MARK, 0, 0);
        qint64 fileSize = additionalInfo.section(SPLIT_ADDITION_INFO_MARK, 1, 1).toLongLong();

        fileName = fileName.right(fileName.size()-fileName.lastIndexOf('/') - 1);

        if (isExistFileInCacheDir(fileName, fileSize))
        {
            send_command(FILE_IS_EXIST_REC_CODE, K_SUCCEED, SUCCEED_CODE_4);
        }
        else
        {
            send_command(FILE_IS_EXIST_REC_CODE, K_ERROR, ERROR_CODE_10);
        }
        break;
    }
    case FILE_IS_DISK_FREE_SPACE_CODE:
    {
        //TODO: 判断添加文件后是否会空间不足;
        qint64 fileSize = additionalInfo.toLongLong();
        QString sreachDir = "";
#ifdef Q_OS_WIN
        sreachDir = _fileCacheDir.left(2);
#elif Q_OS_LINUX
        sreachDir = "/" + _fileCacheDir.section("/", 0, 0);
#endif
        qint64 freeSpaceSize = KFileTransferCacheManage::getDiskFreeSpace(sreachDir);
        if (fileSize <= freeSpaceSize)
        {
            send_command(FILE_IS_DISK_FREE_SPACE_REC_CODE, K_SUCCEED, SUCCEED_CODE_5);
        }
        else
        {
            send_command(FILE_IS_DISK_FREE_SPACE_REC_CODE, K_ERROR, ERROR_CODE_6);
        }
        break;
    }
    }
}

void KFileTransferRecevicer::on_read_file()
{
    if (_startTimeFlag)
    {
        _startTime = QDateTime::currentDateTime();
        _startTimeFlag =false;
    }

    if (_bCancel) return;

    qint64 dataSize = _pFileSocket->bytesAvailable();
    if ((dataSize >= SEND_BLOCK_SIZE) || (dataSize == (_fileSize - _recvSize)))
    {
        QByteArray buf = _pFileSocket->readAll();
        //qint64 len = buf.size();
        qint64 len = _file.write(buf);

        if(len > 0)
        {
           _recvSize += len;
           qDebug()<< "接收的文件数据大小为:" << len / 1024 << "KB" << "\t已接收的数据大小为:" << _recvSize / 1024 << "KB";
        }

        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents); //让系统去处理其他的接收命令
        send_command(FILE_REC_CODE, K_SUCCEED, QString::number(_recvSize));

        if(_recvSize == _fileSize)
        {
            qint64 msec = _startTime.msecsTo(QDateTime::currentDateTime());
            if (msec) qDebug() << "接收数据时间为:" << msec << "ms, \t速率:"<< (_fileSize * 1000) / (1024*1024* msec) << "MB/S";
            QDir::setCurrent(_fileCacheDir);
            _file.close();
        }
    }
}

void KFileTransferRecevicer::onFileError(QAbstractSocket::SocketError)
{
    qDebug() << "file_socket error:" << _pFileSocket->errorString();
}

void KFileTransferRecevicer::onCommandError(QAbstractSocket::SocketError)
{
    qDebug() << "command_socket error:" << _pCommandSocket->errorString();
}
