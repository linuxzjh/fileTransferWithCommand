//#include "stdafx.h"
#include "kfiletransfercachemanage.h"
#include <QStorageInfo>

KFileTransferCacheManage::KFileTransferCacheManage()
{

}

qint64 KFileTransferCacheManage::getDiskFreeSpace(QString driver)
{
    if (driver.isEmpty())
    {
        qDebug() << __FUNCTION__ << "driver content is empty.";
        return 0;
    }
    QStorageInfo  storageInfo(driver);
    storageInfo.refresh();
    return storageInfo.bytesFree();
}
