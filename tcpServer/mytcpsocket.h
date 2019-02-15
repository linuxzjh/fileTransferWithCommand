#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include "config.h"

class KFileTransferRecevicer:public QObject
{
    Q_OBJECT
public:
    KFileTransferRecevicer(QObject *parent = nullptr);
    void send_command(int code, int ret, QString additional = "");
private:
    QTcpSocket *command_socket;
    QTcpSocket *file_socket;
    QTcpServer *tcpServer_c;
    QTcpServer *tcpServer_f;

    QFile file;
    QString filename;
    qint64 filesize;
    qint64 recvSize;
    QDateTime startTime;
    bool flag = true;
public slots:
    void on_read_command();
    void on_read_file();
    void onFileError(QAbstractSocket::SocketError);
    void on_connect_c();
    void on_connect_f();
};

#endif // MYTCPSOCKET_H
