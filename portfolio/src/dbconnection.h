#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql/QSqlDatabase>
#include "stdio.h"

class DBConnection
{
public:

    DBConnection();

    void connect(QString host, QString database, QString user, QString password);
    void disconnet();

private:

    QSqlDatabase db;
};

#endif // DBCONNECTION_H
