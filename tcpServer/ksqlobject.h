#ifndef KSQLOBJECT_H
#define KSQLOBJECT_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class KSQLObject
{
public:
    KSQLObject(const QString& databaseName);
private:
    QSqlDatabase _database;
};

#endif // KSQLOBJECT_H
