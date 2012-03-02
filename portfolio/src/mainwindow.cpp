#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>

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
    QObject::connect(this->ui->btnNext, SIGNAL(clicked()), this, SLOT(switchToAssignmentsPage()));

    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->db.connect("moodlekpm20.db.5672082.hostedresource.com", "moodlekpm20", "moodlekpm20", "Seguro2000!");
    //this->sftp.open("72.167.232.31","kidsplaymath","Seguro2000!");

    this->setAssignmentTableStyle();

    //pdfmerge.mergePdfs("/home/jonathan/Trabajo/KPM/pernambuco/portfolio/bin/","salida.pdf");
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(int download){

    if (download)
        thread = run(this, &MainWindow::downloadHandouts);

    this->ui->stackedWidget->setCurrentIndex(1);
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    //this->handoutsFileNames = this->sftp.getListOfHandouts("html/pdfhandouts/");

    foreach (QString f, this->handoutsFileNames){
        //Sftp conn;
        //conn.open("72.167.232.31","kidsplaymath","Seguro2000!");
        //QFuture<bool> f1 = run(conn, &Sftp::downloadFile, QString("html/pdfhandouts/" + f), f);
        //this->sftp.downloadFile("html/pdfhandouts/" + f, f);
    }
}


void MainWindow::switchToAssignmentsPage(){

    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();
    if(!this->db.userLogin(username, password)){
        qDebug() << "Usuario o contraseña incorretos.";
        return;
    }
    //this->db.printModel();
    //QString userId = this->db.getModel()->record(0).value("id").toString();

    this->ui->stackedWidget->setCurrentIndex(2);
    this->loadAssignments();

    /*this->db.getOnlineFiles(userId);
    this->db.printModel();
    this->db.getUploadFiles(userId);
    this->db.printModel();*/
}

void MainWindow::loadAssignments(){

    QString userId = this->db.getModel()->record(0).value("id").toString();

    this->db.getOnlineFiles(userId);
    QSqlQueryModel *onlineFilesModel = this->db.getModel();
    this->db.getUploadFiles(userId);
    QSqlQueryModel *uploadFilesModel = this->db.getModel();

    this->ui->tableWidgetAssignments->setRowCount(this->handoutsFileNames.count() + onlineFilesModel->rowCount() + uploadFilesModel->rowCount());

    this->thread.waitForFinished();

    int i;

    for (i = 0; i < this->handoutsFileNames.count(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Checked);
        itemName->setText(this->handoutsFileNames.at(i));
        this->ui->tableWidgetAssignments->setItem(i,0,itemPrint);
        this->ui->tableWidgetAssignments->setItem(i,1,itemName);
    }

    for (i = 0; i < onlineFilesModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(onlineFilesModel->record(i).value(1).toString());
        this->ui->tableWidgetAssignments->setItem(i,0,itemPrint);
        this->ui->tableWidgetAssignments->setItem(i,1,itemName);

        //pdfmerge.htmlToPdf(onlineFilesModel->record(i).value(1).toString(), onlineFilesModel->record(i).value(4).toString());
    }

    for (i = 0; i < uploadFilesModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(onlineFilesModel->record(i).value(0).toString());
        this->ui->tableWidgetAssignments->setItem(i,0,itemPrint);
        this->ui->tableWidgetAssignments->setItem(i,1,itemName);

        //sftp.downloadFile(,);
    }

    this->ui->tableWidgetAssignments->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::setAssignmentTableStyle(){

    QStringList header;
    header << "Print?" << "File" << "Date";
    this->ui->tableWidgetAssignments->setColumnCount(3);
    this->ui->tableWidgetAssignments->setHorizontalHeaderLabels(header);
}

MainWindow::~MainWindow()
{
    delete ui;
}
