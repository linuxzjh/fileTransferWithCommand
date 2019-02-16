#include "KFileTransferSender.h"

#include <QtConcurrent/QtConcurrent>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

KFileTransferSender::KFileTransferSender()
{
    command_socket = new QTcpSocket();
    pool.setMaxThreadCount(1);
}

void KFileTransferSender::connect_to_server()
{
    command_socket->connectToHost(QHostAddress(IP),PORT1);
    connect(command_socket,SIGNAL(readyRead()),this,SLOT(on_read_command()));
}

void KFileTransferSender::dis_connect()
{
    command_socket->disconnect();
    command_socket->close();
}

bool KFileTransferSender::set_file(QString filePath)
{
    if(filePath.isEmpty())
    {
        qDebug()<< ERROR_CODE_1;
        return false;
    }
    QFileInfo info(filePath);
    filename = info.fileName();
    filesize = info.size();
    sendSize = 0;

    if(file.isOpen())
    {
        file.close();
    }
    file.setFileName(filePath);
    bool isOk = file.open(QIODevice::ReadOnly);
    if(false == isOk)
    {
        qDebug()<<ERROR_CODE_2;
        return false;
    }
    return true;
}

bool KFileTransferSender::send_command(int type)
{
    QString command;
    switch (type)
    {
        case FILE_HEAD_CODE:
        {
            command = QString("%1##%2##%3").arg(type).arg(filename).arg(filesize);
            break;
        }
        case FILE_CODE:
        {
            command = QString("%1##%2##%3").arg(type).arg("").arg("");
            break;
        }
        case FILE_CANCEL:
        {
            command = QString("%1##%2##%3").arg(type).arg("").arg("");
            break;
        }
        default:
            break;
    }

    qint64 len = command_socket->write(command.toUtf8());
    if(len <= 0)
    {
        qDebug()<< "命令代号:" << type << ERROR_CODE_3;
        file.close();
        return false;
    }
    return true;
}

void KFileTransferSender::send_file()
{
    qint64 len = 0;
    QTcpSocket file_socket;
    file_socket.connectToHost(QHostAddress(IP),PORT2);


    if (! file.isOpen())
    {
        bool bOk = file.open(QIODevice::ReadOnly);
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
        buf = file.read( SEND_BLOCK_SIZE);
        len = file_socket.write(buf);
        file_socket.waitForBytesWritten(-1);
        sendSize += len;

//        qDebug() << "====>发送的数据数据大小:" << len / 1024 << "KB"
//                   << "\t已发送数据大小:" << sendSize / 1024 << "KB";
    } while(len > 0);

    if(sendSize == filesize)
    {
        file.close();
        file_socket.disconnect();
        file_socket.close();

        qDebug() << "文件 " << file.fileName() << "发送完毕 "
                 << QDateTime::currentDateTime().toString("YYYY-MM-DD HH:mm:ss:zzz");
    }
}

void KFileTransferSender::on_read_command()
{
    QByteArray buf = command_socket->readAll();
    int code = QString(buf).section("##",0,0).toInt();
    int ret = QString(buf).section("##",1,1).toInt();
    QString additional = QString(buf).section("##",2,2);

    switch(code)
    {
    //发送文件头响应处理
    case FILE_HEAD_REC_CODE:
    {
#if 0
        ret ? send_file() : qDebug() << ERROR_CODE_4;
#else
        if(ret)
        {
            fileTransferFuture = QtConcurrent::run(&pool, [this] { send_file(); });
        }
        else
        {
            qDebug() << ERROR_CODE_4;
        }
#endif
        break;
    }
    //接收文件数据完成响应处理
    case FILE_REC_CODE:
    {
        if(ret)
        {
            qint64 recv = additional.toLongLong();
            int progressVal = (recv * 100) / filesize;
            emit progressValue(progressVal);
            qDebug() << "文件大小:" << filesize<< ",\t已接收数据大小:" << recv;
        }
        else
        {
            qDebug() << ERROR_CODE_5;
        }
        break;
    }
    //取消文件传输
    case FILE_REC_CANCEL:
    {
        QMessageBox::information(0, "取消成功", "取消成功", QMessageBox::Ok);
        break;
    }
    default:
        break;
    }
}
