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

typedef std::function<void(int&, QString&)> RetCallBack;    //参数：命令执行是否成功,附加信息

class KFileTransferSender:public QObject
{
    Q_OBJECT
public:
    explicit KFileTransferSender(QObject *parent = nullptr);
    void connect_to_server(const QString &ipAddress, int port);                         //连接至服务器
    void sendFile(const QString &filePath, RetCallBack cbFun = nullptr);                //发送文件
    void unSendFile(RetCallBack cbFun = nullptr);                                       //取消文件
    void isExistFile(const QString &filePath, RetCallBack cbFun = nullptr);             //检查文件是否存在
    void isDiskFreeSpace(quint64 reqSpaceSize, RetCallBack cbFun = nullptr);            //检查磁盘空间是否不足， reqSpaceSize--字节
signals:
    void progressValue(int progressVal);                                                //服务端已接受的数据进度
private slots:
    void on_read_command();                                                             //响应服务端信息
private:
    void send_file();                                                                   //发送文件内容数据
    bool send_command(int type, QVariant additionalInfo = "");                          //发送文件信息命令
    bool set_file(const QString& filePath);                                             //设置文件
private:
    QTcpSocket *_pCommandSocket;
    //QTcpSocket *file_socket;

    QFile _file;
    QString _filename;
    qint64 _filesize;
    qint64 _sendSize;

    QThreadPool _pool;
    QFuture<void> _fileTransferFuture;
    QMutex _cancelFileTransferMutex;
    bool _bCancel;

    RetCallBack _cbSendFile;                                                             //发送文件执行回调
    RetCallBack _cbUnSendFile;                                                           //取消文件执行回调
    RetCallBack _cbIsExistFile;                                                          //检查文件是否存在执行回调
    RetCallBack _cbIsFullSpaceWithFile;                                                  //检查磁盘空间是否不足执行回调
};
