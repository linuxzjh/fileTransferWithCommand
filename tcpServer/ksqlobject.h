#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class KSqliteDBAOperator
{
public:
    explicit KSqliteDBAOperator();
    ~KSqliteDBAOperator();
    bool open(const QString& databaseName);     //打开数据库
    bool exec(const QString& sqlCmd);           //数据库指令执行
    bool deleteDB();                            //删除数据库
    bool isExistTable();                        //数据库表是否存在;
private:
    QSqlDatabase _database;
};
