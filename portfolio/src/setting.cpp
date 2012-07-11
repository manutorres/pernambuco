#include "setting.h"

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
    QHostInfo hostInfo = QHostInfo::fromName(SFTP_HOST_NAME);

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
    }
}

QString Setting::getValue(QString attribute){
    return this->xmlSettings.documentElement().elementsByTagName(attribute).at(0).firstChild().nodeValue();
}
