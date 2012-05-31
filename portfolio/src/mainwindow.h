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
#include <QHostInfo>

#include "xmlassigment.h"
#include "pdfmerge.h"
#include "sftp.h"
#include "dbconnection.h"
#include "utils.h"

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
    QList<QPair<QString, QString> > handoutsFileNames;
    QList<QPair<QString, int> > filesToMerge;
    QFuture<void> thread;
    DBConnection db;
    PDFmerge pdfmerge;
    bool downloadsEnabled;
    bool abortConversions;
    int conversionsCount;
    QReadWriteLock conversionsLock;

    void createAppDirectories();
    void clearAppDirectories();
    void setPageTitle(int step, QString task);
    void centerOnScreen();
    void enlargeWindow();
    void reduceWindow();
    bool connectToDatabase();
    void setTreeStyle();
    void setTreeTopLevelItems(QString fileType);
    QTreeWidgetItem* getFileTypeItem(QString type);
    int getTreeNameCount(QString name);
    void getHandoutsFileNames(QString userId);
    void downloadHandouts(QString serverAddress);
    void fillTreeFromUser(int userId);
    void fillTreeFromAssignment();
    void insertOrderedTreeItem(QTreeWidgetItem *parentItem, QTreeWidgetItem *item);
    void setHandoutsToMerge();
    void downloadUploadFiles();
    void convertOnlineFiles();
    void convertForumPostsFiles();
    void mergeFiles();
    void updateParentCheckState(QTreeWidgetItem* item);
    void updateChildrenCheckState(QTreeWidgetItem* item);
    void updatePrintEnabledState();
    void setDownloadsEnabled(bool value);
    void finishDownloadThread(bool hideWindow = false);
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
    void backToLoginPage();
    void backToTreePageFromUser();
    void exit();

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
