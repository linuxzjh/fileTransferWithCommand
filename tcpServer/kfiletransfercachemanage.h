#ifndef KFILETRANSFERCACHEMANAGE_H
#define KFILETRANSFERCACHEMANAGE_H

#include<QObject>
#include<QDebug>

class KFileTransferCacheManage
{
public:
    KFileTransferCacheManage();
    static quint64 getDiskFreeSpace(QString driver); //参数: driver---盘符, 返回值: 返回多少字节
};

#endif // KFILETRANSFERCACHEMANAGE_H
