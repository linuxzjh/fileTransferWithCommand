#pragma once

#include "config.h"
#include "Singleton.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

class KFileTransferRecevicer:public QObject
{
    Q_OBJECT
    DECLARESINGLETON(KFileTransferRecevicer)
public:
    static KFileTransferRecevicer* GetInstance()
    {
        return SINGLETON(KFileTransferRecevicer);
    }
    void send_command(int code, int ret, QString additional = "");
public slots:
    void on_read_command();
    void on_read_file();
    void onFileError(QAbstractSocket::SocketError);
    void onCommandError(QAbstractSocket::SocketError);
    void on_connect_c();
    void on_connect_f();
	void progress();
	void timeOutCheck();
private:
    explicit KFileTransferRecevicer(QObject *parent = nullptr);
    bool isExistFileInCacheDir(const QString& fileName, qint64 fileSize, const QString& md5Str);
    void setCacheDir(const QString& dir);
	
	void beginSave();
    void quitFileTransferThread();
private:
    QTcpSocket *_pCommandSocket;
    QTcpSocket *_pFileSocket;
    QTcpServer *_pTcpServerControl;
    QTcpServer *_pTcpServerFile;

    QFile _file;
	QString _absoluteFilePathInHost;		//文件在主机中的路径，包含文件名
    QString _fileName;						//文件路径名称
    qint64 _fileSize;						//传输文件大小
    qint64 _recvSize;						//接收数据大小
    QString _fileCacheDir;					//文件缓存路径

    bool _bCancel;							//是否取消传输
    QDateTime _startTime;					//传输开始时间记录对象
    bool _startTimeFlag;					//传输开始时间记录标识
	QTimer* m_pTimerProgress;				//定时获取已接收数据大小定时器
	bool m_bRunning;					
	QByteArrayList m_ByteArrayList;			//接收数据缓存队列
	QReadWriteLock m_byteArrayListLock;		//接收数据缓存队列读写锁

	QTimer* m_pProgressTimeOut;				//传输超时监测定时器
	qint64 _recvSize4Before;				//定时检测数据，记录上一次已接收数据大小
};

