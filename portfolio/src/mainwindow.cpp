#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "xmlassigment.h"
#include "pdfmerge.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QObject::connect(this->ui->btnAccept,SIGNAL(clicked()),this,SLOT(switchToLoginPage(true)));
    //QObject::connect(this->ui->btnCancel,SIGNAL(clicked()),this,SLOT(switchToLoginPage(false)));

    /*XMLAssigment obj = XMLAssigment();
    obj.newAssignmentXML();
    obj.addAssignmentElement("online","","","");
    obj.saveFile();

    PDFmerge *pdf = new PDFmerge();*/
}

void MainWindow::switchToLoginPage(bool download){

    //if (download)
       // this->downloadHandouts();

    this->ui->stackedWidget->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
