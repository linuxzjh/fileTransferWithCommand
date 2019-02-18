#pragma once

#include "config.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QMutex>
#include <QFuture>

class KFileTransferSender:public QObject
{
    Q_OBJECT
public:
    explicit KFileTransferSender(QObject *parent = nullptr);
    void connect_to_server(const QString &ipAddress, int port);             //连接至服务器
    void sendFile(const QString &filePath);                                 //发送文件
    void unSendFile();                                                      //取消文件
    bool isExistFile(const QString &file);                                  //检查文件是否存在
    bool isFullSpaceWithFile(const QString &file);                          //检查磁盘空间是否不足
signals:
    void progressValue(int progressVal);                                    //服务端已接受的数据进度
private slots:
    void on_read_command();                                                 //响应服务端信息
private:
    void send_file();                                                       //发送文件内容数据
    bool send_command(int type);                                            //发送文件信息命令
    bool set_file(const QString& filePath);                                 //设置文件
private:
    QTcpSocket *command_socket;
    //QTcpSocket *file_socket;

    QFile file;
    QString filename;
    qint64 filesize;
    qint64 sendSize;

    QThreadPool pool;
    QFuture<void> fileTransferFuture;
    QMutex cancelFileTransferMutex;
    bool bCancel;
};
