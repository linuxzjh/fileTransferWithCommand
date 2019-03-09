#ifndef KFILETRANSFERCACHEMANAGE_H
#define KFILETRANSFERCACHEMANAGE_H

#include<QObject>
#include<QDebug>
#include<QFile>

#include "Singleton.h"

class KFileTransferCacheManage
{
    DECLARESINGLETON(KFileTransferCacheManage)
public:
    static KFileTransferCacheManage* GetInstance()
    {
        return SINGLETON(KFileTransferCacheManage);
    }
	static QString getFileMd5(QFile& file);
    static qint64 getDiskFreeSpace(const QString& driver); //参数: driver---盘符, 返回值: 返回多少字节
private:
    KFileTransferCacheManage();
};

#endif // KFILETRANSFERCACHEMANAGE_H
