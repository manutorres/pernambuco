#include "webmanager.h"
#include "kpm_settings.h"
#include "QDebug"
#include "QWebFrame"

WebManager::WebManager(QObject *parent){
    this->web = new QWebView();
    QObject::connect(this->web, SIGNAL(loadFinished(bool)), this, SLOT(processPage(bool)));
}

void WebManager::getHandouts(int courseId){
    qDebug() << "Course:" << courseId;
    QUrl loginUrl(LOGIN_PAGE_URL);
    loginUrl.addQueryItem("username", "froggy");
    loginUrl.addQueryItem("password", LOGIN_TEST_PASSWORD);
    qDebug() << loginUrl.toString();
    this->web->load(loginUrl);
}

void WebManager::processPage(bool result){
    if (this->web->url().toString() == LOGIN_REDIRECT_PAGE_URL){
        qDebug() << "Successfull login.";
        this->web->load(QUrl(QString("http://kidsplaymath.org/moodle/course/view.php?id=%1").arg("3")));
    }
    if (this->web->url().toString() == "http://kidsplaymath.org/moodle/course/view.php?id=3"){
        qDebug() << this->web->page()->mainFrame()->toHtml();
    }
}
