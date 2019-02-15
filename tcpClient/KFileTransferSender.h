#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QFuture>
#include <QDebug>
#include "config.h"

class KFileTransferSender:public QObject
{
    Q_OBJECT
public:
    KFileTransferSender();
    void connect_to_server();           //连接至服务器
    void dis_connect();                 //取消文件连接
    void send_command(int type);        //发送文件信息命令
    void set_file(QString filePath);    //设置文件
signals:
    void recvSize(qint64 len);          //服务端已接受的数据大小
    void sendFileSize(qint64 fileSize); //发送的文件大小
private slots:
    void on_read_command();             //响应服务端信息
private:
    void send_file();                   //发送文件内容数据
private:
    QTcpSocket *command_socket;
    //QTcpSocket *file_socket;

    QFile file;
    QString filename;
    qint64 filesize;
    qint64 sendSize;

    QThreadPool pool;
    QFuture<void> fileTransferFuture;
};

#endif // MYTCPSOCKET_H
