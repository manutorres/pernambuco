#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->btnPrint->setIcon(QIcon("../ui/print.png"));

    QObject::connect(this->ui->btnNext_1, SIGNAL(clicked()), this, SLOT(switchToLoginPage()));
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(switchToAssignmentsPage()));
    QObject::connect(this->ui->btnNext_3, SIGNAL(clicked()), this, SLOT(switchToProgressPage()));
    QObject::connect(this->ui->btnExit_1, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_2, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_3, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnPrint, SIGNAL(clicked()), this, SLOT(mergeAndPrint()));
    QObject::connect(this->ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(checkProgressBar()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));

    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->setAssignmentTableStyle();
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(){

    if (this->ui->radioButtonHandouts->isChecked())
        thread = run(this, &MainWindow::downloadHandouts);
    else this->ui->progressBar->setRange(0,0);

    this->ui->stackedWidget->setCurrentIndex(1);
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    this->sftp.open(SFTP_HOST_IP, SFTP_USERNAME, SFTP_PASSWORD);
    
    this->handoutsFileNames = this->sftp.getListOfHandouts("html/pdfhandouts/");

    this->ui->progressBar->setRange(0, this->handoutsFileNames.count());

    foreach (QString f, this->handoutsFileNames){
        this->sftp.downloadFile("html/pdfhandouts/" + f, f);
        emit this->downloadedFile();
    }
}

void MainWindow::switchToAssignmentsPage(){

    this->db.connect(MYSQL_HOST_NAME, MYSQL_DATABASE_NAME, MYSQL_USERNAME, MYSQL_PASSWORD);

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
        this->ui->tableWidgetAssignments->setItem(i, 3, itemType);
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

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount() -1; i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() != "handout"))
            countChecked++;
    }
    //Se actualiza la cantidad de archivos a descargar y/o convertir
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + countChecked);

    QFuture<void> th1 = run(this, &MainWindow::downloadUploadFiles);
    QFuture<void> th2 = run(this, &MainWindow::convertOnlineFiles);
    this->ui->stackedWidget->setCurrentIndex(3);
}

//Actualiza la progress bar a medida que se van descargando los archivos y que se van convirtiendo a pdf los assignment online
void MainWindow::updateProgressBar(){

    this->ui->progressBar->setValue(this->ui->progressBar->value() + 1);    
}

void MainWindow::checkProgressBar(){

    this->ui->btnPrint->setEnabled(this->ui->progressBar->value() == this->ui->progressBar->maximum());
}

//Descarga los assignment de tipo upload del servidor
void MainWindow::downloadUploadFiles(){

    Sftp sftp2;
    sftp2.open(SFTP_HOST_IP, SFTP_USERNAME, SFTP_PASSWORD);
    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount() - 1; i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "upload")){
            qDebug() << "A descargar: " << this->sftp.fileHashToPath(this->ui->tableWidgetAssignments->item(i, 4)->text());
            sftp2.downloadFile(this->sftp.fileHashToPath(this->ui->tableWidgetAssignments->item(i, 4)->text()), this->ui->tableWidgetAssignments->item(i, 1)->text());
            this->filesToMerge << this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            emit this->downloadedFile();
        }        
    }    
}

//Convierte a pdf el contenido de los assignment de tipo online de la tabla
void MainWindow::convertOnlineFiles(){

    PDFmerge pdfmerge;

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount() - 1; i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "online")){            
            pdfmerge.htmlToPdf(this->ui->tableWidgetAssignments->item(i, 1)->text(), this->ui->tableWidgetAssignments->item(i, 1)->text());
            this->filesToMerge << this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            emit this->downloadedFile();
        }
    }
}

void MainWindow::mergeAndPrint(){

    //this->pdfmerge.mergePdfs(QDir::currentPath(), "salida.pdf");
    qDebug() << this->filesToMerge;
    qDebug() << this->ui->progressBar->maximum();
}

void MainWindow::exit(){
    this->sftp.disconnect();
    this->db.disconnect();
    QApplication::exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
