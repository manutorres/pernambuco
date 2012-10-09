#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
#include <QHash>

#include "pdfmerge.h"
#include "sftp.h"
#include "dbconnection.h"
#include "utils.h"
#include "setting.h"

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
    static const int TITLE = 0;
    static const int HANDOUT = 1;
    static const int ASSIGNMENT = 2;
    static const int FORUM_POST = 3;

    Sftp sftp;
    DBConnection db;
    PDFmerge pdfmerge;

    int conversionsCount;
    bool downloadsEnabled;
    bool abortConversions;
    bool printingEnabled;
    bool kpmteamLogin;

    QList<QPair<QString, QString> > handoutsFileNames;
    QList<QPair<QString, QString> > titlePagesFileNames;
    QList<QPair<QString, int> > filesToMerge;    
    QFuture<void> downloadThread;
    QReadWriteLock conversionsLock;
    QHash<int, int> hashCourses;//key: numero de item en el combobox. -- value: el id de un curso    
    QHash<int, QString> studentNames;
    QHash<int, QList<QPair<QString, int> > > filesToMergeByStudent; //Estudiante, Archivos a mergear.
    int courseId;

    void setupCourseCheckboxes();
    void createAppDirectories();
    void clearAppDirectories();
    void setPageTitle(int step, QString task);
    void centerOnScreen();
    void enlargeWindow();
    void reduceWindow();
    bool connectToDatabase();
    void resetProgressBar();
    void setTreeStyle();
    void setTreeTopLevelItems(QString fileType);
    QTreeWidgetItem* getFileTypeItem(QString type);
    int getTreeNameCount(QString name);
    bool connectToServer();
    void getHandoutsFileNames(int courseId);
    void getTitlePagesFileNames();
    bool downloadRemoteFiles(QList<QPair<QString, QString> > fileNames);
    void downloadFiles(QList<QPair<QString, QString> > fileNames);

    void fillTreeFromUser(int userId);
    void fillTreeFromAssignment();
    void insertOrderedTreeItem(QTreeWidgetItem *parentItem, QTreeWidgetItem *item);
    void setHandoutsToDownloadAndMerge();
    void downloadUploadFiles();
    void convertOnlineFiles();
    void convertForumPostsFiles();
    void convertCourseAssignments();
    void convertCourseForumPosts();
    void switchPage();
    void mergeFiles(QList<QPair<QString, int> > filesToMerge, QString studentName = "");
    void addHandoutsToMerge();
    QList<QPair<QString, int> > formatFilesToMerge(QList<QPair<QString, QString> > files, int category);

    void updateParentCheckState(QTreeWidgetItem* item);
    void updateChildrenCheckState(QTreeWidgetItem* item);
    void updatePrintEnabledState();
    void setDownloadsEnabled(bool value);
    void finishDownloadThread(bool hideWindow = false);
    void closeEvent(QCloseEvent *event);
    void fillCourses();

private slots:
    void updateCheckState(QTreeWidgetItem *item, int column);
    void switchToLoginPage();
    void loginAndSwitchPage();
    void switchToTreePageFromAssignment();
    void selectAllStudents();
    void selectNoneStudents();
    void switchToCategoriesPage();
    void switchToProgressPage();
    void switchToProgressPageFromCourse();
    void updateProgressBar();
    void checkProgressBar();
    void mergeAndPrint();
    void backToLoginPage();
    void backToFilesPage();
    void backToCoursesPage();   
    void fillStudentsFromCourse();
    void exit();

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
