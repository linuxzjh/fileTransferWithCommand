#ifndef KFILETRANSFERCACHEMANAGE_H
#define KFILETRANSFERCACHEMANAGE_H

#include<QObject>
#include<QDebug>

#include "Singleton.h"

class KFileTransferCacheManage
{
    DECLARESINGLETON(KFileTransferCacheManage)
public:
    static KFileTransferCacheManage* GetInstance()
    {
        return SINGLETON(KFileTransferCacheManage);
    }

    static qint64 getDiskFreeSpace(QString driver); //参数: driver---盘符, 返回值: 返回多少字节
private:
    KFileTransferCacheManage();
};

#endif // KFILETRANSFERCACHEMANAGE_H
