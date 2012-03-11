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

    this->createUserDirectories();

    this->ui->btnPrint->setIcon(QIcon("../ui/print2.png"));
    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgot username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);
    this->setAssignmentTableStyle();

    this->ui->lineEditUsername->setText(LOGIN_TEST_USERNAME);
    this->ui->lineEditPassword->setText(LOGIN_TEST_PASSWORD);

    QObject::connect(this->ui->btnNext_1, SIGNAL(clicked()), this, SLOT(switchToLoginPage()));
    QObject::connect(this->ui->btnLogin, SIGNAL(clicked()), this, SLOT(switchToTablePageFromUser()));
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(switchToTablePageFromAssignment()));
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

    this->finishThread = false;
}

void MainWindow::centerOnScreen(){
    QRect availableGeometry = QDesktopWidget().availableGeometry();
    QRect currentGeometry = this->geometry();
    this->setGeometry(availableGeometry.width() / 2 - currentGeometry.width() / 2,
                      availableGeometry.height() / 2 - currentGeometry.height() / 2,
                      currentGeometry.width(), currentGeometry.height());
}

void MainWindow::enlargeWindow(){
    //Se agranda la ventana verticalmente manteniendola centrada.
    QRect geometry = this->geometry();
    int x = geometry.x();
    int newY = geometry.y() - 50;
    int width = geometry.width();
    int newHeight = geometry.height() + 100;
    this->setGeometry(x, newY, width, newHeight);
}

void MainWindow::setAssignmentTableStyle(){

    QStringList header;
    header << "Print" << "File" << "Date" << "Type" << "Hash";
    this->ui->tableWidgetAssignments->setColumnCount(5);
    this->ui->tableWidgetAssignments->setHorizontalHeaderLabels(header);
    this->ui->tableWidgetAssignments->setColumnWidth(0, 36);
    this->ui->tableWidgetAssignments->setColumnWidth(1, 286);
    this->ui->tableWidgetAssignments->setColumnWidth(2, 81);
    this->ui->tableWidgetAssignments->hideColumn(3);
    //this->ui->tableWidgetAssignments->hideColumn(4);
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

QString MainWindow::timeStampToDate(int unixTime){
    QDateTime timeStamp;
    timeStamp.setTime_t(unixTime);
    return timeStamp.toString("MM-dd-yyyy");
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(){

    if (this->ui->radioButtonHandouts->isChecked())
        thread = run(this, &MainWindow::downloadHandouts);
    else this->ui->progressBar->setRange(0, 0);

    //Lleno el combo con los assignments que tienen por lo menos un assignment_submission online (data1 != '').
    this->db.getOnlineAssignments();
    QSqlQueryModel *onlineAssignments = this->db.getModel();
    for(int i=0; i<onlineAssignments->rowCount(); i++)
        this->ui->cmbAssignments->addItem(onlineAssignments->record(i).value("id").toString());

    this->ui->lblTask->setText("Login");
    this->ui->lblSteps->setText("Step 2 of 4");
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
        if(this->finishThread)
            return;
    }
}

void MainWindow::switchToTablePageFromUser(){
    this->ui->lblLoginFail->setText("");
    //Loading gif

    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();

    if(!this->db.userLogin(username, password)){
        this->ui->lblLoginFail->setText("Invalid username or password.");
        qDebug() << "Usuario o contraseña incorretos.";
        return;
    }        
    this->fillTableFromUser();
    this->ui->lblTask->setText("Assignments selection");
    this->ui->lblSteps->setText("Step 3 of 4");
    this->ui->stackedWidget->setCurrentIndex(2);
    this->enlargeWindow();
}

void MainWindow::switchToTablePageFromAssignment(){
    this->fillTableFromAssignment();
    this->ui->lblTask->setText("Assignments selection");
    this->ui->lblSteps->setText("Step 3 of 4");
    this->ui->stackedWidget->setCurrentIndex(2);
    this->enlargeWindow();
}

void MainWindow::fillTableFromUser(){

    int userId = this->db.getModel()->record(0).value("id").toInt();
    int i;
    int count = 0;

    this->db.getOnlineFilesByUser(userId);
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
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(onlineFilesModel->record(i).value("submission_id").toString() + ". " + onlineFilesModel->record(i).value("name").toString());
        itemDate->setText(this->timeStampToDate(onlineFilesModel->record(i).value(5).toInt()));
        itemType->setText("online");
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 2, itemDate);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
        QString html = "<b>" + onlineFilesModel->record(i).value(1).toString() + "</b><br /><br />" + onlineFilesModel->record(i).value(4).toString();
        this->ui->tableWidgetAssignments->setItem(i + count, 4, new QTableWidgetItem(html));
    }

    count += onlineFilesModel->rowCount();

    this->db.getForumPostsByUser(7);
    QSqlQueryModel *forumPostsModel = this->db.getModel();

    this->ui->tableWidgetAssignments->setRowCount(this->ui->tableWidgetAssignments->rowCount() + forumPostsModel->rowCount());

    for (i = 0; i < forumPostsModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(forumPostsModel->record(i).value("respId").toString() + ". " + forumPostsModel->record(i).value("respSubject").toString());
        itemDate->setText(this->timeStampToDate(forumPostsModel->record(i).value("respModified").toInt()));
        itemType->setText("forum_post");
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 2, itemDate);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
        QString html = "<b>" + forumPostsModel->record(i).value("pregSubject").toString() + "</b>"
                        "<b>" + forumPostsModel->record(i).value("pregMessage").toString() + "</b>"
                        "<br /><br />" + forumPostsModel->record(i).value("respMessage").toString();
        this->ui->tableWidgetAssignments->setItem(i + count, 4, new QTableWidgetItem(html));
    }

    /*
    count += onlineFilesModel->rowCount();

    this->db.getUploadFiles(userId);
    QSqlQueryModel *uploadFilesModel = this->db.getModel();

    this->ui->tableWidgetAssignments->setRowCount(this->ui->tableWidgetAssignments->rowCount() + uploadFilesModel->rowCount());

    for (i = 0; i < uploadFilesModel->rowCount(); i++){

        QTableWidgetItem *itemPrint = new QTableWidgetItem();
        QTableWidgetItem *itemName = new QTableWidgetItem();
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        QTableWidgetItem *itemPathHash = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(uploadFilesModel->record(i).value(0).toString());
        itemDate->setText(this->timeStampToDate(uploadFilesModel->record(i).value(2).toInt()));
        itemType->setText("upload");
        itemPathHash->setText(uploadFilesModel->record(i).value(1).toString());
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 2, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
        this->ui->tableWidgetAssignments->setItem(i + count, 4, itemPathHash);
    }
    */

    //this->ui->tableWidgetAssignments->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::fillTableFromAssignment(){

    int assignmentId = this->ui->cmbAssignments->currentText().toInt();
    int i;
    int count = 0;

    this->db.getOnlineFilesByAssignment(assignmentId);
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
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        QTableWidgetItem *itemType = new QTableWidgetItem();
        itemPrint->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        itemPrint->setCheckState(Qt::Unchecked);
        itemName->setText(onlineFilesModel->record(i).value("submission_id").toString() + ". " + onlineFilesModel->record(i).value("name").toString());
        itemDate->setText(this->timeStampToDate(onlineFilesModel->record(i).value(5).toInt()));
        itemType->setText("online");
        this->ui->tableWidgetAssignments->setItem(i + count, 0, itemPrint);
        this->ui->tableWidgetAssignments->setItem(i + count, 1, itemName);
        this->ui->tableWidgetAssignments->setItem(i + count, 2, itemDate);
        this->ui->tableWidgetAssignments->setItem(i + count, 3, itemType);
        QString html = "<b>" + onlineFilesModel->record(i).value(1).toString() + "</b><br /><br />" + onlineFilesModel->record(i).value(4).toString();
        this->ui->tableWidgetAssignments->setItem(i + count, 4, new QTableWidgetItem(html));
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
    this->convertOnlineFiles();
    this->ui->lblTask->setText("Print");
    this->ui->lblSteps->setText("Step 4 of 4");
    this->ui->stackedWidget->setCurrentIndex(3);    
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
        if ((this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked) &&
           ((this->ui->tableWidgetAssignments->item(i, 3)->text() == "online") ||
           (this->ui->tableWidgetAssignments->item(i, 3)->text() == "forum_post"))){
            QString localFile = this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + this->ui->tableWidgetAssignments->item(i, 1)->text() + ".pdf";
            this->pdfmerge.htmlToPdf(this->ui->tableWidgetAssignments->item(i, 4)->text(), localFile);
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
    return QDir().homePath();
#endif
}

void MainWindow::mergeFiles(){
    QString userDirectory = getUserDirectory();
    QStringList files;
    for (int i = 0; i < this->ui->tableWidgetAssignments->rowCount(); i++){
        if (this->ui->tableWidgetAssignments->item(i, 0)->checkState() == Qt::Checked){
            QString fileName = this->ui->tableWidgetAssignments->item(i, 1)->text();
            QString fileType = this->ui->tableWidgetAssignments->item(i, 3)->text();
            if(fileType == "handout")
                fileName = userDirectory + "/" + HANDOUTS_LOCAL_PATH + "/" + fileName;
            else
                fileName = userDirectory + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + fileName + ".pdf"; //Hola consistencia!

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

void MainWindow::clearDirectory(QString path){
    QDir dir(path);

    foreach (QString file, dir.entryList(QDir::Files))
        dir.remove(file);

}

void MainWindow::exit(){
    if (this->thread.isRunning()){
        this->finishThread = true;
        this->hide();
        this->thread.waitForFinished();
    }
    this->sftp.disconnect();
    this->db.disconnect();
    this->clearDirectory(this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH);
    this->clearDirectory(this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    QApplication::exit();
}


MainWindow::~MainWindow()
{
    delete ui;
}
