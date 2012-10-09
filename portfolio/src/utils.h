#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QDateTime>

class Utils
{
public:

    Utils();
    static void toUnixFile(QString fileName);
    static QString getUserDirectory();
    static void createDirectory(QString path);
    static void clearDirectory(QString path);
    static QString timeStampToDate(int unixTime);
    static bool customSort(QPair<QString, int> item1, QPair<QString, int> item2);
    static QString fileHashToPath(QString fileHash);
    static QString dataToHtml(QString title1, QString text1, QString title2, QString text2, QString title3, QString text3);
};

#endif // UTILS_H
