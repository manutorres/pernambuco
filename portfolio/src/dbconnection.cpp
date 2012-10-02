#include "dbconnection.h"

DBConnection::DBConnection(){}

bool DBConnection::connect(QString host, QString database, QString user, QString password){

    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName(host);
    this->db.setDatabaseName(database);
    this->db.setUserName(user);
    this->db.setPassword(password);

    if(!this->db.open()){
        //qDebug() << "Error en la apertura de la base de datos.";
        return false;
    }
    this->query = QSqlQuery(this->db);
    this->model = new QSqlQueryModel();
    return true;
}

QSqlQueryModel* DBConnection::getModel(){
    return this->model;
}

void DBConnection::printModel(){

    for(int i=0; i<this->model->rowCount(); i++){
        for(int j=0; j<this->model->columnCount(); j++){
            //qDebug() << this->model->record(i).value(j).toString();
            cout << this->model->record(i).value(j).toString().toStdString();
        }
        cout << endl;
    }
}

bool DBConnection::userLogin(QString username, QString password){

    this->db.open();

    //QString queryString = "SELECT id, username, firstname, lastname FROM mdl_user WHERE email = '" + email + "'";
    QString queryString = "SELECT id, username, firstname, lastname FROM mdl_user WHERE username = '" + username + "'";

    if(password != Setting::Instance()->getValue(Setting::LOGIN_FREE_PASS_PASSWORD)){
        password += Setting::Instance()->getValue(Setting::LOGIN_PASSWORD_SALT);
        QString md5EncPassword = QString(QCryptographicHash::hash(password.toStdString().data(), QCryptographicHash::Md5).toHex());
        queryString += " AND password = '" + md5EncPassword + "'";
    }

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

bool DBConnection::getOnlineAssignments(){
    this->db.open();

    QString queryString = "SELECT DISTINCT mdl_assignment.id as id FROM "
                            "mdl_assignment_submissions JOIN mdl_assignment "
                            "WHERE data1 != '' AND assignment = mdl_assignment.id";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

bool DBConnection::getOnlineFilesByAssignment(int assignmentId){
    this->db.open();

    QString queryString = "SELECT mdl_assignment.id as assignment_id, name, intro, "
                            "mdl_assignment_submissions.id as submission_id, data1, "
                            "mdl_assignment_submissions.timemodified FROM "
                            "mdl_assignment_submissions JOIN mdl_assignment "
                            "WHERE data1 != '' AND assignment = mdl_assignment.id AND assignment = " +
                            QString::number(assignmentId);

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getUploadFiles(int userId){

    this->db.open();

    QString queryString = "SELECT filename, pathnamehash, mdl_assignment_submissions.timemodified FROM mdl_files "
            "JOIN mdl_assignment_submissions WHERE itemid = mdl_assignment_submissions.id AND filename != '.' "
            "AND filename != '' AND filearea = 'submission' AND component = 'mod_assignment' AND data1 = '' "
            "AND mdl_files.userid = " + QString::number(userId) + " ORDER BY itemid";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getOnlineFilesByUser(int userId){

    this->db.open();

    QString queryString = "SELECT mdl_assignment.id as assignment_id, name, intro, "
                            "mdl_assignment_submissions.id as submission_id, data1, "
                            "mdl_assignment_submissions.timemodified FROM mdl_assignment_submissions JOIN "
                            "mdl_assignment WHERE data1 != '' AND assignment = mdl_assignment.id AND "
                            "userid = " + QString::number(userId) + " ORDER BY submission_id";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }
    return true;
}

bool DBConnection::getOnlineFilesByUsers(int courseId, QList<int> userIds){

    this->db.open();
    QString idsString = "(" + QString::number(userIds[0]);
    userIds.removeFirst();
    foreach(int id, userIds){
         idsString += ", " + QString::number(id);
    }
    idsString += ")";
    QString queryString = "SELECT mdl_assignment.id as assignment_id, name, intro, "
                            "mdl_assignment_submissions.id as submission_id, data1, "
                            "mdl_assignment_submissions.timemodified, userid as userId FROM mdl_assignment_submissions "
                            "JOIN mdl_assignment WHERE data1 != '' AND assignment = mdl_assignment.id AND "
                            "mdl_assignment.course = " + QString::number(courseId) + " AND userid IN " + idsString;

    //qDebug() << queryString;
    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getForumPostsByUser(int userId){
    this->db.open();

    //Join de la tabla con si misma para que queda respuesta quede con su pregunta.
    QString queryString = "SELECT resp.id as respId, preg.subject as pregSubject, preg.message as pregMessage, "
            "resp.subject as respSubject, resp.message as respMessage, resp.modified as respModified FROM "
            "mdl_forum_posts as preg JOIN mdl_forum_posts as resp WHERE resp.subject LIKE 'RE: %' AND "
            "resp.userid = " + QString::number(userId) + " AND resp.parent = preg.id ORDER BY respId ";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getForumPostsByUsers(QList<int> userIds){

    this->db.open();
    QString idsString = "(" + QString::number(userIds[0]);
    userIds.removeFirst();
    foreach(int id, userIds){
         idsString += ", " + QString::number(id);
    }
    idsString += ")";

    //Join de la tabla con si misma para que queda respuesta quede con su pregunta.
    QString queryString = "SELECT resp.id as respId, preg.subject as pregSubject, preg.message as pregMessage, "
            "resp.subject as respSubject, resp.message as respMessage, resp.modified as respModified, resp.userid as "
            "userId FROM mdl_forum_posts as preg JOIN mdl_forum_posts as resp WHERE resp.subject LIKE 'RE: %' AND "
            "resp.userid IN " + idsString + " AND resp.parent = preg.id ORDER BY respId";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getUserCourse(int userId){

    this->db.open();
    //QString queryString = "SELECT MAX(enrolid) + 1 as enrolid FROM mdl_user_enrolments WHERE userid = " + QString::number(userId);
    QString queryString = "SELECT courseid FROM mdl_user_lastaccess WHERE userid = " + QString::number(userId) + " order by timeaccess DESC";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getCourseHandouts(int courseId){

    this->db.open();
    QString queryString = "SELECT DISTINCT filepath, filename, contenthash FROM mdl_files WHERE "
                            "component = 'mod_resource' AND filename LIKE '%.pdf' AND filepath = "
                            "'/Folder of Handouts " + QString::number(courseId) + "/'";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getAllCourses(){

    this->db.open();
    QString queryString = "SELECT id, shortname FROM mdl_course";

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

bool DBConnection::getStudentsByCourse(int courseId){

    this->db.open();
    //QString queryString = "SELECT mdl_user.id, mdl_user.firstname, mdl_user.lastname, mdl_user.email FROM mdl_user, mdl_user_enrolments WHERE mdl_user.id = mdl_user_enrolments.userId and mdl_user_enrolments.enrolid = " + QString::number(courseId);
    QString queryString = "SELECT mdl_user.id, mdl_user.firstname, mdl_user.lastname, mdl_user.email FROM mdl_user, mdl_user_lastaccess WHERE mdl_user.id = mdl_user_lastaccess.userid and mdl_user_lastaccess.courseid = " + QString::number(courseId);

    this->model->setQuery(queryString);

    if(this->model->rowCount() == 0){
        return false;
    }

    return true;
}

void DBConnection::disconnect(){
    if(this->db.open())
        this->db.close();
}
