#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "xmlassigment.h"
#include "pdfmerge.h"
#include "sftp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->signalMapper = new QSignalMapper(this);
    this->signalMapper->setMapping(this->ui->btnAccept, 1);
    this->signalMapper->setMapping(this->ui->btnCancel, 0);

    QObject::connect(this->ui->btnAccept,SIGNAL(clicked()), this->signalMapper, SLOT(map()));
    QObject::connect(this->ui->btnCancel,SIGNAL(clicked()), this->signalMapper, SLOT(map()));

    QObject::connect(this->signalMapper, SIGNAL(mapped(int)), this, SLOT(switchToLoginPage(int)));


    Sftp *sftp = new Sftp();
    sftp->open("72.167.232.31","kidsplaymath","Seguro2000!");
    sftp->downloadFile("html/pdfhandouts/report.pdf","out.pdf");
    sftp->downloadFile("html/pdfhandouts/encuesta.pdf","out2.pdf");

    /*XMLAssigment obj = XMLAssigment();
    obj.newAssignmentXML();
    obj.addAssignmentElement("online","","","");
    obj.saveFile();

    PDFmerge *pdf = new PDFmerge();*/
}

void MainWindow::switchToLoginPage(int download){

    if (download)
        this->downloadHandouts();

    this->ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::downloadHandouts(){
    printf("entro al metodo downloadHandouts");
}

MainWindow::~MainWindow()
{
    delete ui;
}
