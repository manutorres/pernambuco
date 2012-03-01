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

    QSignalMapper *signalMapper;
    Sftp *sftp;
    QStringList handoutsFileNames;
    QFuture<void> thread;

    void downloadHandouts();

private slots:
    void switchToLoginPage(int download);
};

#endif // MAINWINDOW_H
