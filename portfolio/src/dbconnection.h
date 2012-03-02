#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>
#include <stdio.h>
#include <iostream>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>

#define PASSWORD_SALT "some_very_long_secret!#A12345678901234567890!"
#define UPLOAD_FILES_PATH "kidsplaymath.org/moodle/moodledata/filedir/"

using namespace std;

class DBConnection
{
public:

    DBConnection();
    bool connect(QString host, QString database, QString user, QString password);
    bool userLogin(QString username, QString password);
    bool getOnlineFiles(QString userId);
    bool getUploadFiles(QString userId);
    void disconnect();
    QSqlQueryModel* getModel();
    void printModel();

    //En Sftp
    QString fileHashToPath(QString fileHash);

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;
};

#endif // DBCONNECTION_H
