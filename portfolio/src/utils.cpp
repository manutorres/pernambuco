#include "utils.h"

Utils::Utils()
{
}

//Convierte fines de línea de Windows (\r\n) a Unix (\n).
void Utils::toUnixFile(QString fileName){
    QFile file(fileName);
    QByteArray content;
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "Error al abrir el archivo:" << fileName;
        return;
    }
    while(!file.atEnd()){
        content.append(file.readLine().replace("\r\n", "\n"));
    }
    file.close();
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.close();
}

//Retorna el directorio del usuario dependiendo del SO.
QString Utils::getUserDirectory(){
#ifdef Q_OS_WIN32
    QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
    settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
    return settings.value("Personal").toString().replace("\\", "/");
#else
    return QDir().homePath();
#endif
}

void Utils::createDirectory(QString path){
    QDir().mkpath(path);
}

void Utils::clearDirectory(QString path){
    QDir dir(path);
    foreach (QString file, dir.entryList(QDir::Files))
        dir.remove(file);
}


QString Utils::timeStampToDate(int unixTime){
    QDateTime timeStamp;
    timeStamp.setTime_t(unixTime);
    return timeStamp.toString("MM-dd-yyyy");
}

bool Utils::customSort(QPair<QString, int> item1, QPair<QString, int> item2){
    QString fileItem1 = item1.first.section('/', -1).remove(".pdf");
    QString fileItem2 = item2.first.section('/', -1).remove(".pdf");
    QStringList numbersItem1 = fileItem1.section(' ', 0, 0).split(".");
    QStringList numbersItem2 = fileItem2.section(' ', 0, 0).split(".");
    QString nameItem1 = fileItem1.section(' ', 1);
    QString nameItem2 = fileItem2.section(' ', 1);
    qDebug() << "Name1:" << nameItem1 << numbersItem1 << item1.second;
    qDebug() << "Name2:" << nameItem2 << numbersItem2 << item2.second;

    //Primer número (Topic)
    if(numbersItem1[0].toInt() < numbersItem2[0].toInt())
        return true;
    if(numbersItem1[0].toInt() > numbersItem2[0].toInt())
        return false;

    numbersItem1.pop_front();
    numbersItem2.pop_front();

    //Tipo
    if(item1.second < item2.second)
        return true;
    if(item1.second > item2.second)
        return false;

    //Resto de los números
    for(int i=0; i<numbersItem1.count() && i<numbersItem2.count(); i++){
        if(numbersItem1[i].toInt() < numbersItem2[i].toInt())
            return true;
        if(numbersItem1[i].toInt() > numbersItem2[i].toInt())
            return false;
    }

    //Nombre
    return nameItem1 <= nameItem2;
}

QString Utils::fileHashToPath(QString fileHash){
    return fileHash.left(2) + "/" + fileHash.mid(2, 2) + "/" + fileHash;
}

//Retorna un html listo para ser impreso en pdf.
QString Utils::dataToHtml(QString title, QString intro, QString content){
    QString html =  "<h2 style='font: bold 22px arial, sans-serif;'>" +
                        title +
                    "</h2>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        intro +
                    "</div>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        content +
                    "</div>";
    return html;
}
