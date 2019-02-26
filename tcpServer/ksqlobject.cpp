#include "ksqlobject.h"

#include <QDebug>

KSqliteDBAOperator::KSqliteDBAOperator()
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
}

KSqliteDBAOperator::~KSqliteDBAOperator()
{
    if (_database.isOpen())
    {
        _database.close();
    }
}

bool KSqliteDBAOperator::open(const QString& databaseName)
{
    _database.setDatabaseName(databaseName);
    bool ret = _database.open();
    if (!ret)
    {
        qDebug() << "Error: Failed to connect database." << _database.lastError();
    }
    else
    {
        qDebug() << "Succeed to connect database." ;
    }
    return ret;
}
