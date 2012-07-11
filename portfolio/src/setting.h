#ifndef SETTING_H
#define SETTING_H

#include <QtXml/QDomComment>
#include <QHostInfo>
#include <QString>
#include <QDebug>

#include "kpm_settings.h"
#include "sftp.h"


class Setting
{

public:

    static Setting* Instance();

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
