#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>
#include <stdio.h>
#include <iostream>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>

#define SALT "some_very_long_secret!#A12345678901234567890!"

using namespace std;

class DBConnection
{
public:

    DBConnection();
    bool connect(QString host, QString database, QString user, QString password);
    bool userLogin(QString username, QString password);
    bool getOnlineFiles(QString userId);
    bool getUploadFiles(QString userId);
    void disconnet();
    QSqlQueryModel* getModel();
    void printModel();

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;
};

#endif // DBCONNECTION_H
