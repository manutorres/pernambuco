#include "dbconnection.h"

DBConnection::DBConnection(){}

bool DBConnection::connect(QString host, QString database, QString user, QString password){

    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName(host);
    this->db.setDatabaseName(database);
    this->db.setUserName(user);
    this->db.setPassword(password);

    return this->db.open();
}

bool DBConnection::userLogin(QString username, QString password){

}

void DBConnection::disconnet(){
    this->db.close();
}
