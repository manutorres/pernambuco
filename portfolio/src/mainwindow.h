#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QtConcurrentRun>
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

    void downloadHandouts();
    void setAssignmentTableStyle();
    void loadAssignments();
    void downloadUploadFiles();
    void convertOnlineFiles();
    QString getUserDirectory();
    QStringList getFilesToMergeList();

private slots:
    void switchToLoginPage();
    void switchToAssignmentsPage();
    void switchToProgressPage();
    void updateProgressBar();
    void checkProgressBar();
    void mergeAndPrint();
    void exit();

signals:
    void downloadedFile();    
};

#endif // MAINWINDOW_H
