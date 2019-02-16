#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QMutex>
#include <QFuture>

#include "config.h"
#include "Singleton.h"

class KFileTransferSender:public QObject
{
    Q_OBJECT
    DECLARESINGLETON(KFileTransferSender)
public:
    static KFileTransferSender* GetInstance()
    {
        return SINGLETON(KFileTransferSender);
    }
    void connect_to_server();           //连接至服务器
    bool send_command(int type);        //发送文件信息命令
    bool set_file(QString filePath);    //设置文件
signals:
    void progressValue(int progressVal); //服务端已接受的数据进度
private slots:
    void on_read_command();              //响应服务端信息
private:
    void send_file();                    //发送文件内容数据
    explicit KFileTransferSender(QObject *parent = nullptr);
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

#endif // MYTCPSOCKET_H
