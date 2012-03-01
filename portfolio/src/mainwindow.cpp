#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    //QObject::connect(this->ui->btnNext, SIGNAL(clicked()),this, )

    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->db.connect("moodlekpm20.db.5672082.hostedresource.com","moodlekpm20","moodlekpm20","Seguro2000!");

    /*XMLAssigment obj = XMLAssigment();
    obj.newAssignmentXML();
    obj.addAssignmentElement("online","","","");
    obj.saveFile();

    PDFmerge *pdf = new PDFmerge();*/
}

//Dependiendo de la elección del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(int download){

    if (download)
        thread = run(this, &MainWindow::downloadHandouts);

    this->ui->stackedWidget->setCurrentIndex(1);
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    //this->sftp = new Sftp();
    this->sftp.open("72.167.232.31","kidsplaymath","Seguro2000!");
    this->handoutsFileNames = this->sftp.getListOfHandouts("html/pdfhandouts/");

    foreach (QString f, this->handoutsFileNames){
        //Sftp conn;
        //conn.open("72.167.232.31","kidsplaymath","Seguro2000!");
        //QFuture<bool> f1 = run(conn, &Sftp::downloadFile, QString("html/pdfhandouts/" + f), f);
        this->sftp.downloadFile("html/pdfhandouts/" + f, f);
    }
}


void MainWindow::switchToAssignmentsPage(){

    if (this->db.userLogin(this->ui->lineEditUsername->text(),this->ui->lineEditPassword->text()))
        this->ui->stackedWidget->setCurrentIndex(2);
    else qDebug() << "Fallo login";
}


MainWindow::~MainWindow()
{
    delete ui;
}
