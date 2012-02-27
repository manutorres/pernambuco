#include "dbconnection.h"

DBConnection::DBConnection()
{
    //Se podria directamente crear la conexion acá. Ver.
}

void DBConnection::connect(QString host, QString database, QString user, QString password){

    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName(host);
    this->db.setDatabaseName(database);
    this->db.setUserName(user);
    this->db.setPassword(password);

    //if (!this->db.open())
      //  printf(this->db.lastError());
}

void DBConnection::disconnet(){
    this->db.close();
}
