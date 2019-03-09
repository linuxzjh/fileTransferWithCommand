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
    bool connect_to_server(const QString &ipAddress, int port);                         //连接至服务器
    void sendFile(const QString &filePath);                                             //发送文件
    bool cancelSendFile();                                                              //取消文件
    void isExistFile(QList<checkfileStru>& fileStruList);                               //检查文件是否存在
    bool isDiskFreeSpace(quint64 reqSpaceSize);                                         //检查磁盘空间是否不足
signals:
    void progressValue(const QString &fileName, int progressVal);                       //服务端已接受的数据进度
	void fileTransferFinish();															//文件传输完成信号
	void errorState(int code, int subCode);                                             //错误状态

    void isExistFileRspSig(int type, QVariant retValue);                                //查询文件是否存在响应信号
    void isDiskFreeSpaceRspSig(int type, QVariant retValue);                            //查询磁盘空间是否不足响应信号
    void cancelSendFileRspSig(int type, QVariant retValue);                             //取消文件传输响应信号
private slots:
    void on_read_command();                                                             //响应服务端信息
	void onCommandError(QAbstractSocket::SocketError);									//命令socket错误
	void onFileError(QAbstractSocket::SocketError);										//文件socket错误
private:
    void send_file();                                                                   //发送文件内容数据
    bool send_command(int type, QVariant additionalInfo = "");                          //发送文件信息命令
    bool set_file(const QString& filePath);                                             //设置文件
    void dealReadCommand();
    void quitFileTransferThread();                                                      //退出文件传输线程

private:
    QTcpSocket *_pCommandSocket;

    QFile _file;
    QString _filename;
    qint64 _filesize;
    qint64 _sendSize;
	QString _serverip;
    QThreadPool _pool;
    QFuture<void> _fileTransferFuture;
    QMutex _cancelFileTransferMutex;
    bool _bCancel;
    bool _bRunning;
    qint64 _nextBlockSize;      //下一块大小
    qint64 _recvDataSize;       //已接收数据大小
    QString _cacheData;         //缓存数据容器

    int _timeoutMsec;           //超时时间
	QString m_ipAddress;
};
