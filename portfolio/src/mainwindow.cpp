#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    this->createAppDirectories();

    this->ui->btnPrint->setIcon(QIcon(":/images/greenprinter32.png"));
    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your username or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->ui->lineEditUsername->setText(LOGIN_TEST_USERNAME);
    //this->ui->lineEditUsername->setText("sandalon61");
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
    QObject::connect(this->ui->btnBack_3, SIGNAL(clicked()), this, SLOT(backToLoginPage()));
    QObject::connect(this->ui->btnBack_4, SIGNAL(clicked()), this, SLOT(backToTreePageFromUser()));
    QObject::connect(this, SIGNAL(destroyed()), this, SLOT(exit()));
    QObject::connect(this->ui->btnPrint, SIGNAL(clicked()), this, SLOT(mergeAndPrint()));
    QObject::connect(this->ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(checkProgressBar()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));
    QObject::connect(this->ui->treeWidgetFiles, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(updateCheckState(QTreeWidgetItem*, int)));

    this->setDownloadsEnabled(true);
    this->conversionsCount = 0;
    this->abortConversions = false;

    this->setPageTitle(1, "Login");
    this->ui->btnNext_3->setEnabled(false);
    this->ui->progressBar->setRange(0, 0);
    //this->ui->listWidgetOutputStatus->setEnabled(false);
    this->ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::setDownloadsEnabled(bool value){
    this->downloadsEnabled = value;
}

void MainWindow::finishDownloadThread(bool hideWindow){
    if(!this->thread.isRunning())
        return;
    this->setDownloadsEnabled(false);
    this->sftp.setTransfersEnabled(false);
    if(hideWindow)
        this->hide();
    this->thread.waitForFinished();
    this->sftp.setTransfersEnabled(true);
    this->setDownloadsEnabled(true);
}

void MainWindow::createAppDirectories(){
    Utils::createDirectory(Utils::getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    Utils::createDirectory(Utils::getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH);
    Utils::createDirectory(Utils::getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH);
}

void MainWindow::clearAppDirectories(){
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH);
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH);
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH);
}

void MainWindow::setPageTitle(int step, QString task){
    this->ui->lblTask->setText(task);
    this->ui->lblSteps->setText("Step " + QString::number(step) + " of 3");
}

//Metodo encargado de llevar a cabo la conexion con la base de datos
bool MainWindow::connectToDatabase(){
    if(this->db.connect(MYSQL_HOST_NAME, MYSQL_DATABASE_NAME, MYSQL_USERNAME, MYSQL_PASSWORD)){
        return true;
    }else{
        int ret = QMessageBox::critical(this, "Connection error", "The program was unable to connect to the database.", QMessageBox::Retry, QMessageBox::Abort);
        switch (ret){
        case QMessageBox::Retry :
            return this->connectToDatabase();
            break;
        case QMessageBox::Abort :
            this->exit();
            return false;
        }
    }
}

//Metodo para centrar el mainwindow en la pantalla
void MainWindow::centerOnScreen(){
    QRect rect = this->frameGeometry();
    rect.moveCenter(QDesktopWidget().availableGeometry().center());
    this->move(rect.topLeft());
    qApp->processEvents();
}

//Metodo para acomodar el tamanio de la ventana al momento de desplegar el QTreeWidget
void MainWindow::enlargeWindow(){
    //Se agranda la ventana verticalmente manteniendola centrada.
    QRect geometry = this->geometry();
    int x = geometry.x();    
    int y = geometry.y();
    int width = geometry.width();
    int newHeight = geometry.height() + 100;
    this->setGeometry(x, y, width, newHeight);
    qApp->processEvents();
    this->centerOnScreen();
}

//Metodo para acomodar el tamanio de la ventana al momento de retornar a la pantalla de login.
void MainWindow::reduceWindow(){
    //Se reduce la ventana verticalmente manteniendola centrada.
    QRect geometry = this->geometry();
    int x = geometry.x();
    int y = geometry.y();
    int width = geometry.width();
    int newHeight = geometry.height() - 100;
    this->setGeometry(x, y, width, newHeight);
    qApp->processEvents();
    this->centerOnScreen();

}

//Metodo para setear el estilo del QTreeWidget
void MainWindow::setTreeStyle(){

    QStringList treeHeaders;
    treeHeaders << "File" << "Date" << "PrintData";
    this->ui->treeWidgetFiles->setColumnCount(3);
    this->ui->treeWidgetFiles->hideColumn(2);
    this->ui->treeWidgetFiles->setHeaderLabels(treeHeaders);
    this->ui->treeWidgetFiles->setColumnWidth(0, 320);
    this->ui->treeWidgetFiles->setColumnWidth(1, 66);
    this->ui->treeWidgetFiles->setIconSize(QSize(22, 22));
}

void MainWindow::setTreeTopLevelItems(QString fileType){
    QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, QStringList() << fileType);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(0, Qt::Unchecked);
    item->setIcon(0, QIcon(":/images/folder.png"));
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
    this->updatePrintEnabledState();
    this->ui->treeWidgetFiles->blockSignals(oldState);
}

void MainWindow::updateChildrenCheckState(QTreeWidgetItem* item){
    if(item->checkState(0) == Qt::PartiallyChecked)
        return;
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

void MainWindow::updatePrintEnabledState(){
    QTreeWidgetItem* child;
    for(int i=0; i<this->ui->treeWidgetFiles->topLevelItemCount(); i++){
        child = this->ui->treeWidgetFiles->topLevelItem(i);
        if(child->checkState(0) == Qt::PartiallyChecked || child->checkState(0) == Qt::Checked){
            this->ui->btnNext_3->setEnabled(true);
            return;
        }
    }
    this->ui->btnNext_3->setEnabled(false);
}

//Metodo que dado un item del QTreeWidget retorna si es un Assignment, un Forum Post o un Handout
QTreeWidgetItem* MainWindow::getFileTypeItem(QString type){    
    for(int i=0; i < this->ui->treeWidgetFiles->topLevelItemCount(); i++){        
        if (this->ui->treeWidgetFiles->topLevelItem(i)->text(0) == type){
            return this->ui->treeWidgetFiles->topLevelItem(i);
        }
    }
    return NULL;
}

//Metodo que retorna la cantidad de veces que aparece repetido el nombre de un item dentro del QTreeWidget
int MainWindow::getTreeNameCount(QString name){
    int count = this->ui->treeWidgetFiles->findItems(name, Qt::MatchStartsWith | Qt::MatchRecursive).count();
    return count;
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(){

//    if (this->ui->radioButtonHandouts->isChecked()){
//        thread = run(this, &MainWindow::downloadHandouts);
//    }

    //Lleno el combo con los assignments que tienen por lo menos un assignment_submission online (data1 != '').
    this->db.getOnlineAssignments();
    QSqlQueryModel *onlineAssignments = this->db.getModel();
    for(int i=0; i<onlineAssignments->rowCount(); i++)
        this->ui->cmbAssignments->addItem(onlineAssignments->record(i).value("id").toString());

    this->setPageTitle(1, "Login");
    this->ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::getHandoutsFileNames(QString userId){
    this->handoutsFileNames.clear();
    QString remoteFile;
    QString fileName;
    QSqlQueryModel *model;
    QStringList coursesId;

    this->db.getUserCourse(userId.toInt());
    model = this->db.getModel();

    for (int i = 0; i < model->rowCount(); i++)
        coursesId << model->record(i).value("enrolid").toString();

    foreach (QString courseId, coursesId){
        this->db.getCourseHandouts(courseId.toInt());
        model = this->db.getModel();
        for (int i = 0; i < model->rowCount(); i++){
            remoteFile = QString(HANDOUTS_REMOTE_PATH) + "/" + Utils::fileHashToPath(model->record(i).value("contenthash").toString());
            fileName = model->record(i).value("filename").toString();
            this->handoutsFileNames.append(QPair<QString, QString>(remoteFile, fileName));
        }
    }
    qDebug() << this->handoutsFileNames;
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){
    QString localPath = Utils::getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH;
    QString remoteFile;
    QString localFile;

    this->sftp.open(SFTP_HOST_IP, SFTP_USERNAME, SFTP_PASSWORD);

    for (int i = 0; i < this->handoutsFileNames.count(); i++){
        remoteFile = this->handoutsFileNames.at(i).first;
        localFile = localPath + "/" + this->handoutsFileNames.at(i).second;
        this->sftp.downloadFile(remoteFile, localFile);
        qDebug() << "Downloaded file:" << localFile;
#ifdef Q_OS_WIN32
        Utils::toUnixFile(localFile);
#endif
        emit this->downloadedFile();

        if(!this->downloadsEnabled){
            this->sftp.disconnect();
            return;
        }
    }
    this->sftp.disconnect();
}

void MainWindow::switchToTreePageFromUser(){       
    this->ui->btnNext_2->setEnabled(false);
    if(!connectToDatabase())
        return;
    QString username = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();
    this->ui->lblLoginFail->setStyleSheet("QLabel#lblLoginFail {color: #006EA8;}");
    this->ui->lblLoginFail->setText("Please wait while the application connects to the database...");
    qApp->processEvents();

    if(!this->db.userLogin(username, password)){
        this->ui->lblLoginFail->setStyleSheet("QLabel#lblLoginFail {color: red;}");
        this->ui->lblLoginFail->setText("Your username or password is incorrect. Please try again.");
        this->ui->btnNext_2->setEnabled(true);
        return;
    }
    //Seteo el nombre del archivo de salida ahora que el modelo tiene los datos del usuario.
    QSqlRecord userRecord = this->db.getModel()->record(0);
    QString userId = userRecord.value("id").toString();
    QString outputFile = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/KpmPortfolio " +
            userRecord.value("firstname").toString() + userRecord.value("lastname").toString()[0].toUpper() + ".pdf";
    this->pdfmerge.setOutputFileName(outputFile);
    //qDebug() << "Output file:" << outputFile;

    //Mensaje de descarga de handouts
    //int ret = QMessageBox::question(this, "Handouts download", "Do you want to include the handouts in the portfolio?", QMessageBox::Yes, QMessageBox::No);
    //if(ret == QMessageBox::Yes){
    getHandoutsFileNames(userId);
    thread = run(this, &MainWindow::downloadHandouts);

    this->fillTreeFromUser(userId.toInt());
    this->setPageTitle(2, "File selection");
    this->ui->stackedWidget->setCurrentIndex(3);
    this->enlargeWindow();
}

void MainWindow::switchToTreePageFromAssignment(){
    this->fillTreeFromAssignment();
    this->setPageTitle(2, "Assignment selection");
    this->ui->stackedWidget->setCurrentIndex(2);
    this->enlargeWindow();
}

void MainWindow::fillTreeFromUser(int userId){

    int i, count;
    QStringList itemData;
    QString html, name;

    if (this->handoutsFileNames.count() != 0){
        setTreeTopLevelItems("Handouts");
        QTreeWidgetItem *handouts = this->getFileTypeItem("Handouts");
        handouts->setCheckState(0, Qt::Checked);

        for (i = 0; i < this->handoutsFileNames.count(); i++){            
            itemData << QString(this->handoutsFileNames.at(i).second).replace(".pdf", "");
            QTreeWidgetItem *item = new QTreeWidgetItem(handouts, itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/pdf_file.png"));
            itemData.clear();
        }
    }

    this->db.getOnlineFilesByUser(userId);
    QSqlQueryModel *onlineFilesModel = this->db.getModel();

    if (onlineFilesModel->rowCount() != 0){
        setTreeTopLevelItems("Assignments");
        QTreeWidgetItem *onlineAssignments = this->getFileTypeItem("Assignments");
        onlineAssignments->setCheckState(0, Qt::Checked);

        for (i = 0; i < onlineFilesModel->rowCount(); i++){
            name = onlineFilesModel->record(i).value("name").toString() + " (Text)";
            count = this->getTreeNameCount(name);
            if(count > 0)
                name += " [" + QString::number(count + 1) + "]";
            html = "<h2 style='font: bold 22px arial, sans-serif;'>" +
                        onlineFilesModel->record(i).value("name").toString() +
                    "</h2>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        onlineFilesModel->record(i).value("intro").toString() +
                    "</div>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        onlineFilesModel->record(i).value("data1").toString() +
                    "</div>";

            itemData << name << Utils::timeStampToDate(onlineFilesModel->record(i).value(5).toInt()) << html;
            QTreeWidgetItem *item = new QTreeWidgetItem(onlineAssignments, itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/html_file.png"));
            itemData.clear();
        }
    }

    this->db.getForumPostsByUser(userId);
    QSqlQueryModel *forumPostsModel = this->db.getModel();

    if (forumPostsModel->rowCount() != 0){

        setTreeTopLevelItems("Forum Posts");
        QTreeWidgetItem *forumPosts = this->getFileTypeItem("Forum Posts");
        forumPosts->setCheckState(0, Qt::Checked);

        for (i = 0; i < forumPostsModel->rowCount(); i++){
            name = forumPostsModel->record(i).value("pregSubject").toString();
            count = this->getTreeNameCount(name);
            if(count > 0)
                name += " [" + QString::number(count + 1) + "]";
            html = "<h2 style='font: bold 22px arial, sans-serif;'>" +
                        forumPostsModel->record(i).value("pregSubject").toString() +
                    "</h2>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        forumPostsModel->record(i).value("pregMessage").toString() +
                    "</div>"
                    "<br />"
                    "<div style='font: 20px arial, sans-serif;'>" +
                        forumPostsModel->record(i).value("respMessage").toString() +
                    "</div>";

            qDebug() << forumPostsModel->record(i).value("pregMessage").toString();

            itemData << name << Utils::timeStampToDate(forumPostsModel->record(i).value("respModified").toInt()) << html;
            QTreeWidgetItem *item = new QTreeWidgetItem(forumPosts, itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/forum_file.png"));
            itemData.clear();
        }
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

    /*for (i = 0; i < this->handoutsFileNames.count(); i++){

        itemData << QString(this->handoutsFileNames.at(i).second).replace(".pdf", "");;
        QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, itemData);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(0, Qt::Checked);
        itemData.clear();
    }*/

    for (i = 0; i < onlineFilesModel->rowCount(); i++){

        QString html = "<b>" + onlineFilesModel->record(i).value("name").toString() + "</b>"
                "<br />" + onlineFilesModel->record(i).value("intro").toString();
                "<br /><br />" + onlineFilesModel->record(i).value("data1").toString();
        itemData << onlineFilesModel->record(i).value("name").toString() + " (Text)" <<
                    Utils::timeStampToDate(onlineFilesModel->record(i).value("timemodified").toInt()) <<
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
            //qDebug() << "Iterator:" << (*it)->text(0);
            countChecked++;
        }
        ++it;
    }
    //qDebug() << countChecked;

    //Se actualiza la cantidad de archivos a descargar y/o convertir
    this->ui->progressBar->setRange(0, this->handoutsFileNames.count() + countChecked);
    this->checkProgressBar();

    this->setPageTitle(3, "Print");
    this->ui->stackedWidget->setCurrentIndex(4);
    this->reduceWindow();

    this->filesToMerge.clear();
    this->setHandoutsToMerge();
    if(!this->abortConversions)
        this->convertOnlineFiles();
    if(!this->abortConversions)
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
    QTreeWidgetItemIterator it(this->getFileTypeItem("Assignments"), QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while (*it){        
        //qDebug() << (*it)->text(0);
        if ((*it)->text(0).contains("(Document)")){
            localFile = Utils::getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/"  + QString::number(index) + ". " +
                        (*it)->text(0);
            sftp2.downloadFile(Utils::fileHashToPath((*it)->text(2)), localFile);
            emit this->downloadedFile();
        }
        index++;
        ++it;
    }
}

void MainWindow::setHandoutsToMerge(){    
    QTreeWidgetItem* handoutsItem = this->getFileTypeItem("Handouts");

    if (handoutsItem == NULL)
        return;

    QString localFile;
    QTreeWidgetItemIterator it(handoutsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != handoutsItem)
            break;       
        localFile = Utils::getUserDirectory() + "/" + HANDOUTS_LOCAL_PATH + "/"  + (*it)->text(0) + ".pdf";
        this->filesToMerge << QPair<QString, int>(localFile, MainWindow::HANDOUT);
        ++it;
    }
}

//Convierte a pdf el contenido de los assignment de tipo online de la tabla
void MainWindow::convertOnlineFiles(){    
    QTreeWidgetItem* assignmentsItem = this->getFileTypeItem("Assignments");

    if (assignmentsItem == NULL)
        return;

    QString localFile;
    QTreeWidgetItemIterator it(assignmentsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != assignmentsItem)
            break;       
        if ((*it)->text(0).contains("(Text)")){
            this->conversionsLock.lockForWrite();
            localFile = Utils::getUserDirectory() + "/" + ASSIGNMENTS_LOCAL_PATH + "/" + (*it)->text(0).replace(" (Text)", "") + ".pdf";
            this->pdfmerge.htmlToPdf((*it)->text(2), localFile);
            this->filesToMerge << QPair<QString, int>(localFile, MainWindow::ASSIGNMENT);
            emit this->downloadedFile();
            this->conversionsCount++;
            this->conversionsLock.unlock();
            if(this->abortConversions)
                break;
        }        
        ++it;
    }
}

void MainWindow::convertForumPostsFiles(){    
    QTreeWidgetItem* forumPostsItem = this->getFileTypeItem("Forum Posts");

    if (forumPostsItem == NULL)
        return;

    QString localFile;
    QTreeWidgetItemIterator it(forumPostsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != forumPostsItem)
            break;        
        this->conversionsLock.lockForWrite();
        localFile = Utils::getUserDirectory() + "/" + FORUM_POSTS_LOCAL_PATH + "/" + (*it)->text(0).replace(":", "") + ".pdf";
        this->pdfmerge.htmlToPdf((*it)->text(2), localFile);
        this->filesToMerge << QPair<QString, int>(localFile, MainWindow::FORUM_POST);
        emit this->downloadedFile();
        this->conversionsCount++;
        this->conversionsLock.unlock();
        if(this->abortConversions)
            break;
        ++it;
    }
}

void MainWindow::mergeFiles(){
    qSort(this->filesToMerge.begin(), this->filesToMerge.end(), Utils::customSort);
    //qDebug() << "Files to merge:" << this->filesToMerge;
    QPair<QString, int> file;
    foreach(file, this->filesToMerge){
        QString errorString;
        this->pdfmerge.addPdf(file.first, errorString);
        /*if(this->pdfmerge.addPdf(file.first, errorString)){
            this->ui->listWidgetOutputStatus->addItem("File successfully included: " + file.first.section('/', -2));
        }else{
            QListWidgetItem *item = new QListWidgetItem();
            item->setForeground(QBrush(QColor(255, 0, 0)));
            errorString = errorString.replace(".", ": ") + file.first.section('/', -2);
            item->setText(errorString);
            this->ui->listWidgetOutputStatus->addItem(item);
        }*/
    }
}

void MainWindow::mergeAndPrint(){
    //this->ui->listWidgetOutputStatus->setEnabled(true);
    //this->ui->listWidgetOutputStatus->clear();
    this->mergeFiles();
    if(this->pdfmerge.writeOutput()){
        QMessageBox::information(this, "Successful printing", "Your portfolio has been created and has been saved to your desktop. You can now print it.");
        QDesktopServices::openUrl(QUrl("file:///" + this->pdfmerge.outputFileName()));
    }else{
        QMessageBox::critical(this, "Printing failed", "The program couldn't save the output file.");
    }
}

void MainWindow::backToLoginPage(){
    this->finishDownloadThread();
    qDebug() << "Back to login: downloads finished.";

    this->setPageTitle(1, "Login");
    this->ui->lineEditUsername->setText("");
    this->ui->lineEditPassword->setText("");
    this->ui->lblLoginFail->setText("");
    this->ui->btnNext_2->setEnabled(true);
    this->db.disconnect();
    this->ui->stackedWidget->setCurrentIndex(1);
    this->reduceWindow();
    this->ui->treeWidgetFiles->clear();
    this->ui->progressBar->setValue(0);
    this->clearAppDirectories();
}

void MainWindow::backToTreePageFromUser(){
    this->abortConversions = true;
    this->conversionsLock.lockForRead();
    qDebug() << "Conversions:" << this->conversionsCount;
    this->ui->progressBar->setValue(this->ui->progressBar->value() - this->conversionsCount);
    this->conversionsLock.unlock();
    this->abortConversions = false;
    this->conversionsCount = 0;

    this->setPageTitle(2, "File selection");
    this->ui->stackedWidget->setCurrentIndex(3);
    this->enlargeWindow();

}

void MainWindow::exit(){    
    this->finishDownloadThread(true);
    this->db.disconnect();
    this->clearAppDirectories();
    QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event){
    exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
