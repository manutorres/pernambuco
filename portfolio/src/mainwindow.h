#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QtConcurrentRun>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QFont>

#include "webmanager.h"
#include "xmlassigment.h"
#include "pdfmerge.h"
#include "sftp.h"
#include "dbconnection.h"

namespace Ui {
    class MainWindow;
}

using namespace QtConcurrent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //Constantes para determinar el orden entre tipos de archivo.
    static const int HANDOUT = 0;
    static const int ASSIGNMENT = 1;
    static const int FORUM_POST = 2;

    Sftp sftp;
    QStringList handoutsFileNames;
    QList<QPair<QString, int> > filesToMerge;
    QFuture<void> thread;
    DBConnection db;
    PDFmerge pdfmerge;
    bool finishThread;

    void centerOnScreen();
    void enlargeWindow();
    void connectToDatabase();
    void setTreeStyle();
    void setTreeTopLevelItems(QString fileType);
    QTreeWidgetItem* getFileTypeItem(QString type);
    void createUserDirectories();
    QString timeStampToDate(int unixTime);
    int getTreeNameCount(QString name);
    void downloadHandouts();    
    void fillTreeFromUser();
    void fillTreeFromAssignment();
    void setHandoutsToMerge();
    void downloadUploadFiles();
    void convertOnlineFiles();
    void convertForumPostsFiles();
    QString getUserDirectory();
    void mergeFiles();
    static bool customSort(QPair<QString, int> item1, QPair<QString, int> item2);
    void clearDirectory(QString path);
    void updateParentCheckState(QTreeWidgetItem* item);
    void updateChildrenCheckState(QTreeWidgetItem* item);
    void updatePrintEnabledState();

    void closeEvent(QCloseEvent *event);

private slots:
    void updateCheckState(QTreeWidgetItem *item, int column);
    void switchToLoginPage();
    void switchToTreePageFromUser();
    void switchToTreePageFromAssignment();
    void switchToProgressPage();
    void updateProgressBar();
    void checkProgressBar();
    void mergeAndPrint();
    void exit();
    //Prueba
    void requestFinished(QNetworkReply *reply);

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
