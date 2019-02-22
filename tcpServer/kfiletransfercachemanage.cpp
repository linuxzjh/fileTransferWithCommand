//#include "stdafx.h"
#include "kfiletransfercachemanage.h"
#ifdef Q_OS_WIN
#include <Windows.h>
#else
#include <QStorageInfo>
#endif

KFileTransferCacheManage::KFileTransferCacheManage()
{

}

quint64 KFileTransferCacheManage::getDiskFreeSpace(QString driver)
{
    if (driver.isEmpty())
    {
        qDebug() << __FUNCTION__ << "driver content is empty.";
        return 0;
    }
#ifdef Q_OS_WIN
    LPCWSTR lpcwstrDriver=(LPCWSTR)driver.utf16();
    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
    if( !GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
    {
     qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
     return 0;
    }
    return (quint64) liTotalFreeBytes.QuadPart;
#else
#ifdef Q_OS_LINUX
    QStorageInfo  storageInfo(driver);
    storageInfo.refresh();
    return storageInfo.bytesFree();
#endif
#endif
}
