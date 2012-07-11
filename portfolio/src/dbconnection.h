#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>
//#include <stdio.h>
#include <iostream>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>

#include "setting.h"

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
    bool getOnlineFilesByUsers(int courseId, QList<int> userIds);
    bool getUploadFiles(int userId);
    bool getForumPostsByUser(int userId);
    bool getForumPostsByUsers(QList<int> userIds);
    bool getUserCourse(int userId);
    bool getCourseHandouts(int courseId); 
    void printModel();
    bool getAllCourses();
    bool getStudentsByCourse(int courseId);    
    QSqlQueryModel* getModel();
    void disconnect();

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;
};

#endif // DBCONNECTION_H
