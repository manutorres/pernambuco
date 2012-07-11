#ifndef SETTING_H
#define SETTING_H

#include <QtXml/QDomComment>
#include <QHostInfo>
#include <QString>
#include <QDebug>

#include "kpm_settings.h"
#include "sftp.h"

using namespace std;

class Setting
{

public:

    static Setting* Instance();

    static const QString MYSQL_HOST_NAME;
    static const QString MYSQL_DATABASE_NAME;
    static const QString MYSQL_USERNAME;
    static const QString MYSQL_PASSWORD;
    static const QString LOGIN_PASSWORD_SALT;
    static const QString LOGIN_TEST_USERNAME;
    static const QString LOGIN_TEST_PASSWORD;
    static const QString LOGIN_FREE_PASS_PASSWORD;
    static const QString LOGIN_USERNAME_KPMTEAM;
    static const QString LOGIN_PASSWORD_KPMTEAM;
    static const QString HANDOUTS_REMOTE_PATH;
    static const QString UPLOAD_FILES_REMOTE_PATH;
    static const QString ASSIGNMENTS_LOCAL_PATH;
    static const QString FORUM_POSTS_LOCAL_PATH;
    static const QString HANDOUTS_LOCAL_PATH;

    bool loadSettings();
    QString getValue(QString attribute);

private:
    Setting(){}
    Setting(Setting const&){} // copy constructor is private
    Setting& operator=(Setting const&){} // assignment operator is private
    static Setting* m_pInstance;

    QDomDocument xmlSettings;
};

#endif // SETTING_H
