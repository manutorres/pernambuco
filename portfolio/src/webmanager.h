#ifndef WEBMANAGER_H
#define WEBMANAGER_H
#include <QWebView>

class WebManager : QObject
{
    Q_OBJECT

public:
    WebManager(QObject *parent = 0);
    void getHandouts(int courseId);
    ~WebManager();

private:
    QWebView *web;


private slots:
    void processPage(bool result);
};

#endif // WEBMANAGER_H
