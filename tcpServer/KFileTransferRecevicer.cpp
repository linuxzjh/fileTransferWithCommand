//#include "stdafx.h"
#include "KFileTransferRecevicer.h"

#include<QtConcurrent/QtConcurrent>

KFileTransferRecevicer::KFileTransferRecevicer(QObject *parent)
    : QObject(parent)
    , bCancel(false)
{
    command_socket = new QTcpSocket(this);
    file_socket = new QTcpSocket(this);
    tcpServer_c = new QTcpServer(this);
    tcpServer_f = new QTcpServer(this);
    file_socket->setReadBufferSize(64*1024*1024);

    tcpServer_c->listen(QHostAddress::Any,PORT_COMMAND);
    tcpServer_f->listen(QHostAddress::Any,PORT_FILE);

    connect(tcpServer_c,SIGNAL(newConnection()),this,SLOT(on_connect_c()));
    connect(tcpServer_f,SIGNAL(newConnection()),this,SLOT(on_connect_f()));
}

void KFileTransferRecevicer::on_connect_c(){
    command_socket = tcpServer_c->nextPendingConnection();

    QString ip = command_socket->peerAddress().toString();
    quint16 port = command_socket->peerPort();

    qDebug()<<QString("[%1:%2]成功连接").arg(ip).arg(port);
    command_socket->disconnect();
    connect(command_socket,SIGNAL(readyRead()),this,SLOT(on_read_command()));
    connect(command_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onCommandError(QAbstractSocket::SocketError)));
}

void KFileTransferRecevicer::on_connect_f(){
    file_socket = tcpServer_f->nextPendingConnection();

    QString ip = file_socket->peerAddress().toString();
    quint16 port = file_socket->peerPort();

    qDebug()<<QString("[%1:%2]成功连接").arg(ip).arg(port);
    file_socket->disconnect();
    connect(file_socket,SIGNAL(readyRead()),this,SLOT(on_read_file()));
    connect(file_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onFileError(QAbstractSocket::SocketError)));
    flag = true;
}

void KFileTransferRecevicer::send_command(int code, int ret, QString additional)
{
    QString head = QString("%1##%2##%3").arg(code).arg(ret).arg(additional);
    qint64 len = command_socket->write(head.toUtf8());
    if(len <= 0)
    {
        qDebug()<<"命令发送失败 "<<code;
    }
}

void KFileTransferRecevicer::on_read_command()
{
    QByteArray buf = command_socket->readAll();
    int code = QString(buf).section("##",0,0).toInt();
    switch (code)
    {
        case FILE_HEAD_CODE:
        {
            filename = QString(buf).section("##",1,1);
            filesize = QString(buf).section("##",2,2).toLongLong();
            recvSize = 0;
            bCancel = false;

            /////////////////////////////////////////////////
            //TODO: 这里判断
            //1.磁盘的大小是否足够，
            //2.文件是否已经存在等信息

            file.setFileName(filename);
            bool isOk = file.open(QIODevice::WriteOnly);
            if(false == isOk)
            {
                qDebug()<<"writeonly error 49";
                command_socket->disconnectFromHost();
                command_socket->close();
                return;
            }

            qDebug()<<QString("文件名：%1\n大小:%2 kb").arg(filename).arg(filesize/1024);
            send_command(FILE_HEAD_REC_CODE, K_SUCCEED, SUCCEED_CODE_1);
            break;
        }
        case FILE_CODE:
        {
            //TODO: 开始接收文件数据信息;
            //
            qDebug() << "开始接收文件数据信息 " << QDateTime::currentDateTime().toString("YYYY-MM-DD HH:mm:ss:zzz");
            break;
        }
        case FILE_CANCEL:
        {
            //TODO: 取消文件当前传输文件时需要做的操作;
            send_command(FILE_REC_CANCEL, K_SUCCEED, SUCCEED_CODE_3);
            file.close();
            bCancel = true;
            //file_socket->disconnect();
            //file_socket->close();
            break;
        }
    }
}

void KFileTransferRecevicer::on_read_file()
{
    if (flag)
    {
        startTime = QDateTime::currentDateTime();
        flag =false;
    }

    if (bCancel) return;

    qint64 dataSize = file_socket->bytesAvailable();
    if ((dataSize >= SEND_BLOCK_SIZE) || (dataSize == (filesize - recvSize)))
    {
        QByteArray buf = file_socket->readAll();
        //qint64 len = buf.size();
        qint64 len = file.write(buf);

        if(len > 0)
        {
           recvSize += len;
           qDebug()<< "接收的文件数据大小为:" << len / 1024 << "KB" << "\t已接收的数据大小为:" << recvSize / 1024 << "KB";
        }

        send_command(FILE_REC_CODE, K_SUCCEED, QString::number(recvSize));

        if(recvSize == filesize)
        {
            qint64 msec = startTime.msecsTo(QDateTime::currentDateTime());
            if (msec) qDebug() << "接收数据时间为:" << msec << "ms, \t速率:"<< (filesize * 1000) / (1024*1024* msec) << "MB/S";
            file.close();
            //file_socket->disconnect();
            //file_socket->close();
        }
    }
}

void KFileTransferRecevicer::onFileError(QAbstractSocket::SocketError)
{
    qDebug() << "file_socket error:" << file_socket->errorString();
}

void KFileTransferRecevicer::onCommandError(QAbstractSocket::SocketError)
{
    qDebug() << "command_socket error:" << command_socket->errorString();
}
