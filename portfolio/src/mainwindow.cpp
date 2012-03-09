#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDesktopWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->centerOnScreen();

    this->ui->btnPrint->setIcon(QIcon("../ui/print2.png"));
    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);
    this->setAssignmentTableStyle();

    this->ui->lineEditUsername->setText(LOGIN_TEST_USERNAME);
    this->ui->lineEditPassword->setText(LOGIN_TEST_PASSWORD);

    QObject::connect(this->ui->btnNext_1, SIGNAL(clicked()), this, SLOT(switchToLoginPage()));
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(switchToAssignmentsPage()));
    QObject::connect(this->ui->btnNext_3, SIGNAL(clicked()), this, SLOT(switchToProgressPage()));
    QObject::connect(this->ui->btnExit_1, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_2, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_3, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_4, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnPrint, SIGNAL(clicked()), this, SLOT(mergeAndPrint()));
    QObject::connect(this->ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(checkProgressBar()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));
    QObject::connect(this->ui->btnSelectAll, SIGNAL(clicked()), this, SLOT(selectAllFiles()));
    QObject::connect(this->ui->btnSelectNone, SIGNAL(clicked()), this, SLOT(selectNoneFiles()));

    this->sftp.open(SFTP_HOST_IP, SFTP_USERNAME, SFTP_PASSWORD);
    this->db.connect(MYSQL_HOST_NAME, MYSQL_DATABASE_NAME, MYSQL_USERNAME, MYSQL_PASSWORD);
}

void MainWindow::createButtons(){
    //this->ui->buttonBox1->addButton(this->ui->btnNext_1, QDialogButtonBox::ActionRole);
    //this->ui->btnNext_1->setDefault(true);
    //this->ui->buttonBox1->addButton(this->ui->btnExit_1, QDialogButtonBox::ActionRole);
}

void MainWindow::centerOnScreen(){
    QRect availableGeometry = QDesktopWidget().availableGeometry();
    QRect currentGeometry = this->geometry();
    this->setGeometry(availableGeometry.width() / 2 - currentGeometry.width() / 2,
                      availableGeometry.height() / 2 - currentGeometry.height() / 2,
                      currentGeometry.width(), currentGeometry.height());
}

void MainWindow::setAssignmentTableStyle(){

    QStringList header;
    header << "Print" << "File" << "Date" << "Type" << "Hash";
    this->ui->tableWidgetAssignments->setColumnCount(5);
    this->ui->tableWidgetAssignments->setHorizontalHeaderLabels(header);
    this->ui->tableWidgetAssignments->setColumnWidth(0, 10);
    this->ui->tableWidgetAssignments->setColumnWidth(1, 250);
    this->ui->tableWidgetAssignments->setColumnWidth(0, 50);
    this->ui->tableWidgetAssignments->hideColumn(3);
    this->ui->tableWidgetAssignments->hideColumn(4);
}

void MainWindow::createUserDirectories(){
    QDir().mkpath(this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    QDir().mkpath(this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH);
}

void MainWindow::selectAllFiles(){
    for (int i=0; i<this->ui->tableWidgetAssignments->rowCount(); i++)
        this->ui->tableWidgetAssignments->item(i, 0)->setCheckState(Qt::Checked);
}

void MainWindow::selectNoneFiles(){
    for (int i=0; i<this->ui->tableWidgetAssignments->rowCount(); i++)
        this->ui->tableWidgetAssignments->item(i, 0)->setCheckState(Qt::Unchecked);
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(){

    if (this->ui->radioButtonHandouts->isChecked())
        thread = run(this, &MainWindow::downloadHandouts);
    else this->ui->progressBar->setRange(0, 0);

    this->ui->stackedWidget->setCurrentIndex(1);
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    this->handoutsFileNames = this->sftp.getListOfHandouts(HANDOUTS_REMOTE_PATH);

    this->ui->progressBar->setRange(0, this->handoutsFileNames.count());

    QString localPath = this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH;
    foreach (QString f, this->handoutsFileNames){
        QString remoteFile = QString(HANDOUTS_REMOTE_PATH) + "/" + f;
        QString localFile = localPath + "/" + f;
        qDebug() << "Remote:" << remoteFile;
        qDebug() << "Local:" << localFile;
        this->sftp.downloadFile(remoteFile, localFile);
        emit this->downloadedFile();
    }
}

void MainWindow::switchToAssignmentsPage(){
    this->ui->lblLoginFail->setText("");
    //Loading gif

    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();

    if(!this->db.userLogin(username, password)){
        this->ui->lblLoginFail->setText("Your username or password is incorrect. Please try again.");
        qDebug() << "Usuario o contraseña incorretos.";
        return;
    }        
    this->loadAssignments();    
    this->ui->stackedWidget->setCurrentIndex(2);

    //Se agranda la ventana verticalmente manteniendola centrada.
    QRect geometry = this->geometry();
    int x = geometry.x();
    int newY = geometry.y() - 60;
    int width = geometry.width();
    int newHeight = geometry.height() + 120;
    this->setGeometry(x, newY, width, newHeight);
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

    count += onlineFilesModel->rowCount();

    this->db.getUploadFiles(userId);
    QSqlQueryModel *uploadFilesModel = this->db.getModel();

    this->ui->tableWidgetAssignments->setRowCount(this->ui->tableWidgetAssignments->rowCount() + uploadFilesModel->rowCount());

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

    //this->ui->tableWidgetAssignments->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::switchToProgressPage(){

    int countChecked = 0;//sin tener en cuenta los handouts

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() != "handout"))
            countChecked++;
    }
    //Se actualiza la cantidad de archivos a descargar y/o convertir
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + countChecked);
    this->checkProgressBar();

    QFuture<void> th1 = run(this, &MainWindow::downloadUploadFiles);
    //QFuture<void> th2 = run(this, &MainWindow::convertOnlineFiles);
    this->ui->stackedWidget->setCurrentIndex(3);
    this->convertOnlineFiles();
}

//Actualiza la progress bar a medida que se van descargando los archivos y que se van convirtiendo a pdf los assignment online
void MainWindow::updateProgressBar(){
    qDebug() << "Update progress bar.";
    qDebug() << "Maximum:" << this->ui->progressBar->maximum();
    qDebug() << "Value:" << this->ui->progressBar->value();
    this->ui->progressBar->setValue(this->ui->progressBar->value() + 1);    
}

void MainWindow::checkProgressBar(){

    this->ui->btnPrint->setEnabled(this->ui->progressBar->value() == this->ui->progressBar->maximum());
}

//Descarga los assignment de tipo upload del servidor
void MainWindow::downloadUploadFiles(){

    Sftp sftp2;
    sftp2.open(SFTP_HOST_IP, SFTP_USERNAME, SFTP_PASSWORD);
    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "upload")){
            qDebug() << "A descargar: " << this->sftp.fileHashToPath(this->ui->tableWidgetAssignments->item(i, 4)->text());
            QString localFile = this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + this->ui->tableWidgetAssignments->item(i, 1)->text();
            sftp2.downloadFile(this->sftp.fileHashToPath(this->ui->tableWidgetAssignments->item(i, 4)->text()), localFile);
            //this->filesToMerge << this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            emit this->downloadedFile();
        }        
    }    
}

//Convierte a pdf el contenido de los assignment de tipo online de la tabla
void MainWindow::convertOnlineFiles(){
    qDebug() << "convertOnlineFiles";
    PDFmerge pdfmerge;

    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) && (this->ui->tableWidgetAssignments->item(i, 3)->text() == "online")){            
            QString localFile = this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            this->pdfmerge.htmlToPdf(this->ui->tableWidgetAssignments->item(i, 1)->text(), localFile);
            //this->filesToMerge << this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            emit this->downloadedFile();
        }
    }
}

QString MainWindow::getUserDirectory(){
#ifdef Q_OS_WIN32
    QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
    settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
    return settings.value("Personal").toString().replace("\\", "/");
#endif
#ifdef Q_OS_UNIX
    return QDir().homePath();
#endif
#ifdef Q_OS_MAC
#endif
}

void MainWindow::mergeFiles(){
    QString userDirectory = getUserDirectory();
    QStringList files;
    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if (this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked){
            QString fileName = this->ui->tableWidgetAssignments->item(i, 1)->text();
            QString fileType = this->ui->tableWidgetAssignments->item(i, 3)->text();
            if(fileType == "handout"){
                fileName = userDirectory + "/" + HANDOUTS_LOCAL_PATH + "/" + fileName;
            }else{
                fileName = userDirectory + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + fileName + ".pdf"; //Hola consistencia!
            }
            QString errorString;
            if(this->pdfmerge.addPdf(fileName, errorString)){
                this->ui->listWidgetOutputStatus->addItem("File successfully included: " + fileName);
            }else{
                QListWidgetItem *item = new QListWidgetItem();
                item->setForeground(QBrush(QColor(255, 0, 0)));
                item->setText(errorString);
                this->ui->listWidgetOutputStatus->addItem(item);
            }
        }
    }
}

void MainWindow::mergeAndPrint(){
    this->mergeFiles();
    QString outputFile = this->getUserDirectory() + "/" + OUTPUT_LOCAL_FILE;
    if(this->pdfmerge.writeOutput(outputFile)){
        QMessageBox::information(this, "Printing finished", "The output file was successfully created.");
    }else{
        //Ver si se puede sacar alguna conclusión con un archivo de Qt.
        QMessageBox::critical(this, "Printing failed", "The program couldn't save the output file.");
    }
}

void MainWindow::exit(){

    //VER BIEN COMO CERRAR DEPENDIENDO DE LA PESTAÑA Y QUE NO SE ROMPA

    qDebug() << "Exit 1";
    this->sftp.disconnect();
    qDebug() << "Exit 2";
    this->db.disconnect();
    qDebug() << "Exit 3";
    QApplication::exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
