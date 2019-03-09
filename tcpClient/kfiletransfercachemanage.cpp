//#include "stdafx.h"
#include "kfiletransfercachemanage.h"
#include <QStorageInfo>
#include <QCryptographicHash>

KFileTransferCacheManage::KFileTransferCacheManage()
{

}

qint64 KFileTransferCacheManage::getDiskFreeSpace(const QString& driver) 
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

QString KFileTransferCacheManage::getFileMd5(QFile& file)
{
	if (!file.open(QIODevice::ReadOnly))
		return QByteArray();
	QByteArray context = file.readAll();
	file.close();
	return QString(QCryptographicHash::hash(context, QCryptographicHash::Md5).toHex());
}
