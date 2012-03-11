#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>
#include <stdio.h>
#include <iostream>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>
#include "kpm_settings.h"

#define PASSWORD_SALT "some_very_long_secret!#A12345678901234567890!"

using namespace std;

class DBConnection
{
public:

    DBConnection();
    bool connect(QString host, QString database, QString user, QString password);
    bool userLogin(QString username, QString password);
    bool getOnlineAssignments();
    bool getOnlineFilesByAssignment(int assignmentId);
    bool getOnlineFilesByUser(int userId);
    bool getUploadFiles(int userId);
    bool getForumPostsByUser(int userId);
    void disconnect();
    QSqlQueryModel* getModel();
    void printModel();

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;
};

#endif // DBCONNECTION_H
