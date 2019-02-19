#include "kfiletransfercachemanage.h"
#include <Windows.h>

KFileTransferCacheManage::KFileTransferCacheManage()
{

}

quint64 KFileTransferCacheManage::getDiskFreeSpace(QString driver)
{
    LPCWSTR lpcwstrDriver=(LPCWSTR)driver.utf16();
    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
    if( !GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
    {
     qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
     return 0;
    }
    return (quint64) liTotalFreeBytes.QuadPart;
}
