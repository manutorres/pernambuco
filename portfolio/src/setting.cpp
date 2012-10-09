#include "setting.h"

const QString Setting::SSH_HOST_NAME = QString("SSH_HOST_NAME");
const QString Setting::SSH_USERNAME = QString("SSH_USERNAME");
const QString Setting::SSH_PASSWORD = QString("SSH_PASSWORD");
const QString Setting::MYSQL_HOST_NAME = QString("MYSQL_HOST_NAME");
const QString Setting::MYSQL_DATABASE_NAME = QString("MYSQL_DATABASE_NAME");
const QString Setting::MYSQL_USERNAME = QString("MYSQL_USERNAME");
const QString Setting::MYSQL_PASSWORD = QString("MYSQL_PASSWORD");
const QString Setting::LOGIN_PASSWORD_SALT = QString("LOGIN_PASSWORD_SALT");
const QString Setting::LOGIN_TEST_USERNAME = QString("LOGIN_TEST_USERNAME");
const QString Setting::LOGIN_TEST_PASSWORD = QString("LOGIN_TEST_PASSWORD");
const QString Setting::LOGIN_FREE_PASS_PASSWORD = QString("LOGIN_FREE_PASS_PASSWORD");
const QString Setting::LOGIN_USERNAME_KPMTEAM = QString("LOGIN_USERNAME_KPMTEAM");
const QString Setting::LOGIN_PASSWORD_KPMTEAM = QString("LOGIN_PASSWORD_KPMTEAM");
const QString Setting::HANDOUTS_REMOTE_PATH = QString("HANDOUTS_REMOTE_PATH");
const QString Setting::UPLOAD_FILES_REMOTE_PATH = QString("UPLOAD_FILES_REMOTE_PATH");
const QString Setting::TITLE_PAGES_REMOTE_PATH = QString("TITLE_PAGES_REMOTE_PATH");
const QString Setting::ASSIGNMENTS_LOCAL_PATH = QString("ASSIGNMENTS_LOCAL_PATH");
const QString Setting::FORUM_POSTS_LOCAL_PATH = QString("FORUM_POSTS_LOCAL_PATH");
const QString Setting::HANDOUTS_LOCAL_PATH = QString("HANDOUTS_LOCAL_PATH");
const QString Setting::TITLE_PAGES_LOCAL_PATH = QString("TITLE_PAGES_LOCAL_PATH");

// Global static pointer used to ensure a single instance of the class.
Setting* Setting::m_pInstance = NULL;

/* This function is called to create an instance of the class.
   Calling the constructor publicly is not allowed. The constructor
   is private and is only called by this Instance function.
*/
Setting* Setting::Instance(){
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new Setting;

   return m_pInstance;
}

bool Setting::loadSettings(){
    /*QHostInfo hostInfo = QHostInfo::fromName(SFTP_HOST_NAME);

    if(hostInfo.addresses().isEmpty()){
        return false;
    }
    else{
        bool result;

        QString serverAddress = hostInfo.addresses().first().toString();
        Sftp sftp;

        result = sftp.open(serverAddress, SFTP_USERNAME, SFTP_PASSWORD);
        QString settingsContent = sftp.downloadFileContent("KpmSettings/printportfolioconfig.xml");

        this->xmlSettings.clear();
        result = result && this->xmlSettings.setContent(settingsContent);

        sftp.disconnect();

        return result;
    }*/

    this->settingsLoaded = false;
    QObject::connect(&this->ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(ftpCommandFinished(int,bool)));
    ftp.connectToHost(SFTP_HOST_NAME, 21);
    ftp.login(SFTP_USERNAME, SFTP_PASSWORD);
    QEventLoop loop;
    QObject::connect(this, SIGNAL(finishLoop()), &loop, SLOT(quit()));
    loop.exec();
    return this->settingsLoaded;
}

QString Setting::getValue(QString attribute){
    return this->xmlSettings.documentElement().elementsByTagName(attribute).at(0).firstChild().nodeValue();
}

void Setting::ftpCommandFinished(int, bool error){

    if (ftp.currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            this->settingsLoaded = false;
            emit finishLoop();
            return;
        }

        this->settingsContent.open(QIODevice::ReadWrite);        
        ftp.get(SFTP_SETTINGS_PATH, &this->settingsContent);
        return;
    }

    if (ftp.currentCommand() == QFtp::Login){
        if (error){
            this->settingsLoaded = false;
            emit finishLoop();
            return;
        }
    }

    if (ftp.currentCommand() == QFtp::Get) {
        if (error) {
            this->settingsLoaded = false;
            emit finishLoop();
        }
        else {            
            this->xmlSettings.clear();
            QString content = QString(this->settingsContent.data());
            this->xmlSettings.setContent(content);
            this->settingsLoaded = true;
            emit finishLoop();
        }
    }
}
