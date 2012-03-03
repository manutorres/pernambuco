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
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(switchToProgressPage()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));

    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->db.connect("moodlekpm20.db.5672082.hostedresource.com", "moodlekpm20", "moodlekpm20", "Seguro2000!");
    this->sftp.open("72.167.232.31","kidsplaymath","Seguro2000!");

    this->setAssignmentTableStyle();
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(int download){

    if (download)
        thread = run(this, &MainWindow::downloadHandouts);

    this->ui->stackedWidget->setCurrentIndex(1);
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    this->handoutsFileNames = this->sftp.getListOfHandouts("html/pdfhandouts/");

    this->ui->progressBar->setRange(0, this->handoutsFileNames.count() -1);

    foreach (QString f, this->handoutsFileNames){
        this->sftp.downloadFile("html/pdfhandouts/" + f, f);
        emit this->downloadedFile();
    }
}


void MainWindow::switchToAssignmentsPage(){

    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();

    if(!this->db.userLogin(username, password)){
        qDebug() << "Usuario o contraseña incorretos.";
        return;
    }        

    this->ui->stackedWidget->setCurrentIndex(2);
    this->loadAssignments();    
}

void MainWindow::loadAssignments(){

    QString userId = this->db.getModel()->record(0).value("id").toString();
    int i;
    int count = 0;

    this->db.getOnlineFiles(userId);
    QSqlQueryModel *onlineFilesModel = this->db.getModel();    

    this->ui->tableWidgetAssignments->setRowCount(this->handoutsFileNames.count() + onlineFilesModel->rowCount());

    for (i = 0; i < this->handoutsFileNames.count(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Checked);
        itemName->setText(this->handoutsFileNames.at(i));
        itemType->setText("handout");
        this->ui->tableWidgetAssignments->setItem(i, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i, 1, itemName);
    }

    count = this->handoutsFileNames.count();

    for (i = 0; i < onlineFilesModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(onlineFilesModel->record(i).value(1).toString());
        itemType->setText("online");
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
    }

    this->db.getUploadFiles(userId);
    QSqlQueryModel *uploadFilesModel = this->db.getModel();

    this->ui->tableWidgetAssignments->setRowCount(this->ui->tableWidgetAssignments->rowCount() + uploadFilesModel->rowCount());

    count += onlineFilesModel->rowCount();

    for (i = 0; i < uploadFilesModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        QTableWidgetItem *itemPathHash = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(uploadFilesModel->record(i).value(0).toString());
        itemType->setText("upload");
        itemPathHash->setText(uploadFilesModel->record(i).value(1).toString());
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
        this->ui->tableWidgetAssignments->setItem(i + count, 4, itemPathHash);
    }

    this->ui->tableWidgetAssignments->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::setAssignmentTableStyle(){

    QStringList header;
    header << "Print?" << "File" << "Date" << "Type" << "filePathHash";
    this->ui->tableWidgetAssignments->setColumnCount(5);
    this->ui->tableWidgetAssignments->setHorizontalHeaderLabels(header);
    this->ui->tableWidgetAssignments->hideColumn(3);
    this->ui->tableWidgetAssignments->hideColumn(4);
}

void MainWindow::switchToProgressPage(){

    int countChecked = 0;//sin tener en cuenta los handouts

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 0)->text() != "handout"))
            countChecked++;
    }
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + countChecked - 1);

    this->ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::updateProgressBar(){

    this->ui->progressBar->setValue(this->ui->progressBar->value() + 1);
}

void MainWindow::downloadUploadFiles(){

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "upload")){
            sftp.downloadFile(this->sftp.fileHashToPath(this->ui->tableWidgetAssignments->item(i, 1)->text()), this->ui->tableWidgetAssignments->item(i, 1)->text());
            emit this->downloadedFile();
        }
    }
}

void MainWindow::convertOnlineFiles(){
    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "online")){
            pdfmerge.htmlToPdf(this->ui->tableWidgetAssignments->item(i, 1)->text(), this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf");
            emit this->downloadedFile();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
