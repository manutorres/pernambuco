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
#include <QHash>

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
    QHash<int, int> hashCourses;//key: numero de item en el combobox. -- value: el id de un curso
    bool kpmteamLogin;
    QHash<int, QString> studentNames;
    QHash<int, QList<QPair<QString, int> > > filesToMergeByStudent; //Estudiante, Archivos a mergear.
    bool printingEnabled;

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
    void getHandoutsFileNames(int courseId);
    void downloadHandouts(QString serverAddress);
    void downloadSettingsFile();
    bool downloadCourseHandouts(int courseId);
    void fillTreeFromUser(int userId);
    void fillTreeFromAssignment();
    void insertOrderedTreeItem(QTreeWidgetItem *parentItem, QTreeWidgetItem *item);
    void setHandoutsToMerge();
    void downloadUploadFiles();
    void convertOnlineFiles();
    void convertForumPostsFiles();
    void convertCourseAssignments();
    void convertCourseForumPosts();
    void switchPage();
    void mergeFiles(QList<QPair<QString, int> > filesToMerge, QString StudentName = "");
    void addHandoutsToMerge();
    QList<QPair<QString, int> >  addHandoutsToMergeInMultipleReport();//Es para el caso de kpmteam, en el cual hay más de 5 estudiantes (reportes)
                                                                      //Con lo que genera esta función se llama mergeFiles(...)
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
    void backToTreePageFromUser();
    void backToCoursesPage();
    void exit();
    void fillStudentsFromCourse();

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
