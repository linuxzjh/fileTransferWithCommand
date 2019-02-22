#include "ksqlobject.h"

#include <QDebug>

KSQLObject::KSQLObject(const QString& databaseName)
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName(databaseName);
    if (!_database.open())
    {
        qDebug() << "Error: Failed to connect database." << _database.lastError();
    }
    else
    {
        qDebug() << "Succeed to connect database." ;
    }
}
