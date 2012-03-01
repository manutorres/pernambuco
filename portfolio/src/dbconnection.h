#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QString>
#include <stdio.h>

class DBConnection
{
public:

    DBConnection();

    bool connect(QString host, QString database, QString user, QString password);
    bool userLogin(QString username, QString password);
    void disconnet();

private:

    QSqlDatabase db;
};

#endif // DBCONNECTION_H
