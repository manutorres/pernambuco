#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QtConcurrentRun>
#include <QTreeWidgetItem>
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

    Sftp sftp;
    QStringList handoutsFileNames, filesToMerge;
    QFuture<void> thread;
    DBConnection db;
    PDFmerge pdfmerge;
    bool finishThread;

    void centerOnScreen();
    void enlargeWindow();
    void setFilesTreeStyle();
    void setFilesTreeTopItems();
    QTreeWidgetItem* getFileTypeItem(QString type);
    void createUserDirectories();
    QString timeStampToDate(int unixTime);
    void downloadHandouts();    
    void fillTreeFromUser();
    void fillTreeFromAssignment();
    void downloadUploadFiles();
    void convertOnlineFiles();
    void convertForumPostsFiles();
    QString getUserDirectory();
    void mergeFiles();
    void clearDirectory(QString path);

private slots:
    void switchToLoginPage();
    void switchToTreePageFromUser();
    void switchToTreePageFromAssignment();
    void switchToProgressPage();
    void updateProgressBar();
    void checkProgressBar();
    void mergeAndPrint();
    void exit();

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
