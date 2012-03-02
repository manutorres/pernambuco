#include "dbconnection.h"

DBConnection::DBConnection(){}

bool DBConnection::connect(QString host, QString database, QString user, QString password){

    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName(host);
    this->db.setDatabaseName(database);
    this->db.setUserName(user);
    this->db.setPassword(password);

    if(!this->db.open()){
        qDebug() << "Error en la apertura de la base de datos.";
        return false;
    }
    this->query = QSqlQuery(this->db);
    this->model = new QSqlQueryModel();
}

QSqlQueryModel* DBConnection::getModel(){
    return this->model;
}

void DBConnection::printModel(){
    qDebug() << "Rows:" << this->model->rowCount();
    qDebug() << "Columns:" << this->model->columnCount();
    for(int i=0; i<this->model->rowCount(); i++){
        for(int j=0; j<this->model->columnCount(); j++){
            qDebug() << this->model->record(i).value(j).toString();
            //cout << this->model->record(i).value(j).toString().toStdString();
        }
        cout << endl;
    }
}

bool DBConnection::userLogin(QString username, QString password){
    username = "froggy";
    password = "D*S4RbPOJ@";
    password += PASSWORD_SALT;
    QString encPasswordMd5 = QString(QCryptographicHash::hash(password.toStdString().data(), QCryptographicHash::Md5).toHex());
    QString queryString = "SELECT id, username, firstname, lastname FROM mdl_user "
                            "WHERE username = '" + username + "' AND password = '" + encPasswordMd5 + "'";

    //qDebug() << "Username:" << username;
    //qDebug() << "Password:" << password;
    //qDebug() << "Password Md5:" << encPasswordMd5;

    this->model->setQuery(queryString);
    qDebug() << this->model->lastError();
    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

bool DBConnection::getOnlineFiles(QString userId){

    // mdl_assignment_submission tiene timecreated, timemodified y timemarked

    QString queryString = "SELECT mdl_assignment.id as assignment_id, name, intro, "
                            "mdl_assignment_submissions.id as submission_id, data1 FROM "
                            "mdl_assignment_submissions JOIN mdl_assignment "
                            "WHERE assignment = mdl_assignment.id AND userid = " + userId;
    this->model->setQuery(queryString);
    qDebug() << this->model->lastError();
    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

bool DBConnection::getUploadFiles(QString userId){

    // mdl_files tiene timecreated y timemodified

    QString queryString = "SELECT filename, pathnamehash FROM mdl_files "
                            "WHERE filearea = 'submission' AND userid = " + userId;
    this->model->setQuery(queryString);
    qDebug() << this->model->lastError();
    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

void DBConnection::disconnect(){
    this->db.close();
}

QString DBConnection::fileHashToPath(QString fileHash){
    return UPLOAD_FILES_PATH + fileHash.left(2) + "/" + fileHash.mid(2, 2) + "/" + fileHash;
}
