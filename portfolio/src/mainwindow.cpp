#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDesktopWidget>
#include <QMessageBox>

const int MainWindow::HANDOUT;
const int MainWindow::ASSIGNMENT;
const int MainWindow::FORUM_POST;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->centerOnScreen();
    this->setTreeStyle();

    this->createUserDirectories();

    this->ui->btnPrint->setIcon(QIcon("../ui/print2.png"));
    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgot username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->ui->lineEditUsername->setText(LOGIN_TEST_USERNAME);
    this->ui->lineEditPassword->setText(LOGIN_TEST_PASSWORD);

    QObject::connect(this->ui->btnNext_1, SIGNAL(clicked()), this, SLOT(switchToLoginPage()));
    //QObject::connect(this->ui->btnLogin, SIGNAL(clicked()), this, SLOT(switchToTreePageFromUser()));
    //QObject::connect(this->ui->btnNext_2b, SIGNAL(clicked()), this, SLOT(switchToTreePageFromAssignment()));
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(switchToTreePageFromUser()));
    QObject::connect(this->ui->btnNext_3, SIGNAL(clicked()), this, SLOT(switchToProgressPage()));
    QObject::connect(this->ui->btnExit_1, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_2, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_3, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_4, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this, SIGNAL(destroyed()), this, SLOT(exit()));
    QObject::connect(this->ui->btnPrint, SIGNAL(clicked()), this, SLOT(mergeAndPrint()));
    QObject::connect(this->ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(checkProgressBar()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));
    QObject::connect(this->ui->treeWidgetFiles, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(updateCheckState(QTreeWidgetItem*, int)));

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

void MainWindow::setTreeStyle(){

    QStringList treeHeaders;
    treeHeaders << "File" << "Date" << "PrintData";
    this->ui->treeWidgetFiles->setColumnCount(3);
    this->ui->treeWidgetFiles->hideColumn(2);
    this->ui->treeWidgetFiles->setHeaderLabels(treeHeaders);
    this->ui->treeWidgetFiles->setColumnWidth(0, 320);
    this->ui->treeWidgetFiles->setColumnWidth(1, 66);
}

void MainWindow::setTreeTopLevelItems(){
    QStringList fileTypes;
    fileTypes << "Handouts" << "Assignments" << "Forum Posts";
    foreach(QString type, fileTypes){
        QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, QStringList() << type);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Unchecked);
    }
}

void MainWindow::updateCheckState(QTreeWidgetItem* item, int column){
    if(column != 0)
        return;
    bool oldState = this->ui->treeWidgetFiles->blockSignals(true);
    if(!item->parent()){ //item es top-level
        this->updateChildrenCheckState(item);
    }else{
        this->updateParentCheckState(item);
    }
    this->ui->treeWidgetFiles->blockSignals(oldState);
}

void MainWindow::updateChildrenCheckState(QTreeWidgetItem* item){
    if(item->checkState(0) == Qt::PartiallyChecked)
        return;
    QTreeWidgetItem* child;
    for(int i=0; i<item->childCount(); i++){
        item->child(i)->setCheckState(0, item->checkState(0));
    }
}

void MainWindow::updateParentCheckState(QTreeWidgetItem* item){
    QTreeWidgetItem* child;
    for(int i=0; i<item->parent()->childCount(); i++){
        child = item->parent()->child(i);
        if((child != item) && (child->checkState(0) != item->checkState(0))){
            item->parent()->setCheckState(0, Qt::PartiallyChecked);
            return;
        }
    }
    item->parent()->setCheckState(0, item->checkState(0));
}

QTreeWidgetItem* MainWindow::getFileTypeItem(int type){
    return this->ui->treeWidgetFiles->topLevelItem(type);
}

int MainWindow::getTreeNameCount(QString name){
    int count = this->ui->treeWidgetFiles->findItems(name, Qt::MatchStartsWith | Qt::MatchRecursive).count();
    return count;
}

void MainWindow::createUserDirectories(){
    QDir().mkpath(this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    QDir().mkpath(this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH);
    QDir().mkpath(this->getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH);
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

void MainWindow::switchToTreePageFromUser(){
    this->ui->lblLoginFail->setText("");
    //Loading gif

    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();

    if(!this->db.userLogin(username, password)){
        this->ui->lblLoginFail->setText("Your username or password is incorrect. Please try again.");
        qDebug() << "Usuario o contraseña incorretos.";
        return;
    }        
    this->setTreeTopLevelItems();
    this->fillTreeFromUser();
    this->ui->lblTask->setText("Files selection");
    this->ui->lblSteps->setText("Step 3 of 4");
    this->ui->stackedWidget->setCurrentIndex(3);
    this->enlargeWindow();
}

void MainWindow::switchToTreePageFromAssignment(){
    this->fillTreeFromAssignment();
    this->ui->lblTask->setText("Assignments selection");
    this->ui->lblSteps->setText("Step 3 of 4");
    this->ui->stackedWidget->setCurrentIndex(2);
    this->enlargeWindow();
}

void MainWindow::fillTreeFromUser(){

    int userId = this->db.getModel()->record(0).value("id").toInt();
    int i;

    this->db.getOnlineFilesByUser(userId);

    QSqlQueryModel *onlineFilesModel = this->db.getModel();    
    QTreeWidgetItem *handouts = this->getFileTypeItem(HANDOUT);
    handouts->setCheckState(0, Qt::Checked);
    QStringList itemData;
    QString html, name;
    int count;

    for (i = 0; i < this->handoutsFileNames.count(); i++){
        itemData << QString(this->handoutsFileNames.at(i)).replace(".pdf", "");
        QTreeWidgetItem *item = new QTreeWidgetItem(handouts, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Checked);
        itemData.clear();
    }    

    QTreeWidgetItem *onlineAssignments = this->getFileTypeItem(ASSIGNMENT);

    for (i = 0; i < onlineFilesModel->rowCount(); i++){
        name = onlineFilesModel->record(i).value("name").toString() + " (Text)";
        count = this->getTreeNameCount(name);
        if(count > 0)
            name += " [" + QString::number(count + 1) + "]";
        html = "<b>" + onlineFilesModel->record(i).value("name").toString() + "</b>"
                "<br />" + onlineFilesModel->record(i).value("intro").toString();
                "<br /><br />" + onlineFilesModel->record(i).value("data1").toString();

        itemData << name << this->timeStampToDate(onlineFilesModel->record(i).value(5).toInt()) << html;
        QTreeWidgetItem *item = new QTreeWidgetItem(onlineAssignments, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Unchecked);
        itemData.clear();

    }

    this->db.getForumPostsByUser(userId);
    QSqlQueryModel *forumPostsModel = this->db.getModel();
    QTreeWidgetItem *forumPosts = this->getFileTypeItem(FORUM_POST);

    for (i = 0; i < forumPostsModel->rowCount(); i++){
        name = forumPostsModel->record(i).value("pregSubject").toString();
        count = this->getTreeNameCount(name);
        if(count > 0)
            name += " [" + QString::number(count + 1) + "]";
        html = "<b>" + forumPostsModel->record(i).value("pregSubject").toString() + "</b>"
                "<br />" + forumPostsModel->record(i).value("pregMessage").toString() +
                "<br /><br />" + forumPostsModel->record(i).value("respMessage").toString();

        itemData << name << this->timeStampToDate(forumPostsModel->record(i).value("respModified").toInt()) << html;
        QTreeWidgetItem *item = new QTreeWidgetItem(forumPosts, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Unchecked);
        itemData.clear();
    }

    /*
    this->db.getUploadFiles(userId);
    QSqlQueryModel *uploadFilesModel = this->db.getModel();

    for (i = 0; i < uploadFilesModel->rowCount(); i++){

        itemData << uploadFilesModel->record(i).value("filename").toString() <<
                    this->timeStampToDate(uploadFilesModel->record(i).value("timemodified").toInt()) <<
                    uploadFilesModel->record(i).value("pathnamehash").toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(forumPosts, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Unchecked);
        itemData.clear();
    }
    */
}

void MainWindow::fillTreeFromAssignment(){

    int assignmentId = this->ui->cmbAssignments->currentText().toInt();
    int i;

    this->db.getOnlineFilesByAssignment(assignmentId);
    QSqlQueryModel *onlineFilesModel = this->db.getModel();
    QStringList itemData;

    for (i = 0; i < this->handoutsFileNames.count(); i++){

        itemData << this->handoutsFileNames.at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Checked);
        itemData.clear();
    }

    for (i = 0; i < onlineFilesModel->rowCount(); i++){

        QString html = "<b>" + onlineFilesModel->record(i).value("name").toString() + "</b>"
                "<br />" + onlineFilesModel->record(i).value("intro").toString();
                "<br /><br />" + onlineFilesModel->record(i).value("data1").toString();
        itemData << onlineFilesModel->record(i).value("name").toString() + " (Text)" <<
                    this->timeStampToDate(onlineFilesModel->record(i).value("timemodified").toInt()) <<
                    html;
        QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Unchecked);
        itemData.clear();
    }
}

void MainWindow::switchToProgressPage(){

    int countChecked = 0;//sin tener en cuenta los handouts

    QTreeWidgetItemIterator it(this->ui->treeWidgetFiles, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while (*it){
        if((*it)->parent()->text(0) != "Handouts"){
            qDebug() << "Iterator:" << (*it)->text(0);
            countChecked++;
        }
        ++it;
    }
    qDebug() << countChecked;

    //Se actualiza la cantidad de archivos a descargar y/o convertir
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + countChecked);
    this->checkProgressBar();

    this->ui->lblTask->setText("Print");
    this->ui->lblSteps->setText("Step 4 of 4");
    this->ui->stackedWidget->setCurrentIndex(4);
    //QFuture<void> th1 = run(this, &MainWindow::downloadUploadFiles);
    this->setHandoutsToMerge();
    this->convertOnlineFiles();
    this->convertForumPostsFiles();
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
    QString localFile;
    int index = 0;
    QTreeWidgetItemIterator it(this->getFileTypeItem(ASSIGNMENT), QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while (*it){        
        qDebug() << (*it)->text(0);
        if ((*it)->text(0).contains("(Document)")){
            localFile = this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/"  + QString::number(index) + ". " +
                        (*it)->text(0);
            sftp2.downloadFile(this->sftp.fileHashToPath((*it)->text(2)), localFile);
            emit this->downloadedFile();
        }
        index++;
        ++it;
    }
}

void MainWindow::setHandoutsToMerge(){
    QString localFile;
    QTreeWidgetItem* handoutsItem = this->getFileTypeItem(HANDOUT);
    QTreeWidgetItemIterator it(handoutsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != handoutsItem)
            break;       
        localFile = this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH + "/"  + (*it)->text(0) + ".pdf";        
        this->filesToMerge << QPair<QString, int>(localFile, MainWindow::HANDOUT);
        ++it;
    }
}

//Convierte a pdf el contenido de los assignment de tipo online de la tabla
void MainWindow::convertOnlineFiles(){
    QString localFile;    
    QTreeWidgetItem* assignmentsItem = this->getFileTypeItem(ASSIGNMENT);
    QTreeWidgetItemIterator it(assignmentsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != assignmentsItem)
            break;       
        if ((*it)->text(0).contains("(Text)")){
            localFile = this->getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + (*it)->text(0) + ".pdf";
            this->pdfmerge.htmlToPdf((*it)->text(2), localFile);
            this->filesToMerge << QPair<QString, int>(localFile, MainWindow::ASSIGNMENT);
            emit this->downloadedFile();
        }        
        ++it;
    }
}

void MainWindow::convertForumPostsFiles(){
    QString localFile;
    QTreeWidgetItem* forumPostsItem = this->getFileTypeItem(FORUM_POST);
    QTreeWidgetItemIterator it(forumPostsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != forumPostsItem)
            break;        
        localFile = this->getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH + "/" + (*it)->text(0).replace(":", "") + ".pdf";
        this->pdfmerge.htmlToPdf((*it)->text(2), localFile);
        this->filesToMerge << QPair<QString, int>(localFile, MainWindow::FORUM_POST);
        emit this->downloadedFile();        
        ++it;
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

    qSort(this->filesToMerge.begin(), this->filesToMerge.end(), customSort);
    qDebug() << "Files to merge:" << this->filesToMerge;
    QPair<QString, int> file;
    foreach(file, this->filesToMerge){
        QString errorString;
        if(this->pdfmerge.addPdf(file.first, errorString)){
            this->ui->listWidgetOutputStatus->addItem("File successfully included: " + file.first);
        }else{
            QListWidgetItem *item = new QListWidgetItem();
            item->setForeground(QBrush(QColor(255, 0, 0)));
            item->setText(errorString);
            this->ui->listWidgetOutputStatus->addItem(item);
        }
    }
}

bool MainWindow::customSort(QPair<QString, int> item1, QPair<QString, int> item2){
    QString fileItem1 = item1.first.section('/', -1);
    QString fileItem2 = item2.first.section('/', -1);
    QStringList numbersItem1 = fileItem1.section(' ', 0, 0).split(".");
    QStringList numbersItem2 = fileItem2.section(' ', 0, 0).split(".");
    QString nameItem1 = fileItem1.section(' ', 1);
    QString nameItem2 = fileItem2.section(' ', 1);
    qDebug() << "Name1:" << nameItem1 << numbersItem1 << item1.second;
    qDebug() << "Name2:" << nameItem2 << numbersItem2 << item2.second;

    for(int i=0; i<numbersItem1.count() && i<numbersItem2.count(); i++){
        if(numbersItem1[i].toInt() < numbersItem2[i].toInt())
            return true;
        if(numbersItem1[i].toInt() > numbersItem2[i].toInt())
            return false;
    }
    if(item1.second < item2.second)
        return true;
    if(item1.second > item2.second)
        return false;
    return nameItem1 <= nameItem2;
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
    this->clearDirectory(this->getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH);
    this->clearDirectory(this->getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    QApplication::exit();
}


MainWindow::~MainWindow()
{
    delete ui;
}
