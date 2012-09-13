#include "mainwindow.h"
#include "ui_mainwindow.h"

const int MainWindow::HANDOUT;
const int MainWindow::ASSIGNMENT;
const int MainWindow::FORUM_POST;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);

    if (!Setting::Instance()->loadSettings()){
        QMessageBox::critical(this, "Connection failed", "The program couldn't connect to the server.");
    }

    QObject::connect(this->ui->btnNext_1, SIGNAL(clicked()), this, SLOT(switchToLoginPage()));
    //QObject::connect(this->ui->btnLogin, SIGNAL(clicked()), this, SLOT(switchToTreePageFromUser()));
    //QObject::connect(this->ui->btnNext_2b, SIGNAL(clicked()), this, SLOT(switchToTreePageFromAssignment()));
    QObject::connect(this->ui->btnNext_2, SIGNAL(clicked()), this, SLOT(loginAndSwitchPage()));
    QObject::connect(this->ui->btnNext_3, SIGNAL(clicked()), this, SLOT(switchToProgressPage()));
    QObject::connect(this->ui->btnNext_5, SIGNAL(clicked()), this, SLOT(switchToCategoriesPage()));
    QObject::connect(this->ui->btnNext_6, SIGNAL(clicked()), this, SLOT(switchToProgressPageFromCourse()));
    QObject::connect(this->ui->btnBack_3, SIGNAL(clicked()), this, SLOT(backToLoginPage()));
    QObject::connect(this->ui->btnBack_4, SIGNAL(clicked()), this, SLOT(backToTreePageFromUser()));
    QObject::connect(this->ui->btnBack_5, SIGNAL(clicked()), this, SLOT(backToLoginPage()));
    QObject::connect(this->ui->btnBack_6, SIGNAL(clicked()), this, SLOT(backToCoursesPage()));
    QObject::connect(this->ui->btnExit_1, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_2, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_3, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_4, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_5, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this->ui->btnExit_6, SIGNAL(clicked()), this, SLOT(exit()));
    QObject::connect(this, SIGNAL(destroyed()), this, SLOT(exit()));
    QObject::connect(this->ui->btnPrint, SIGNAL(clicked()), this, SLOT(mergeAndPrint()));
    QObject::connect(this->ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(checkProgressBar()));
    QObject::connect(this, SIGNAL(downloadedFile()), this, SLOT(updateProgressBar()));
    QObject::connect(this->ui->treeWidgetFiles, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(updateCheckState(QTreeWidgetItem*, int)));
    QObject::connect(this->ui->btnSelectAll, SIGNAL(clicked()), this, SLOT(selectAllStudents()));
    QObject::connect(this->ui->btnSelectNone, SIGNAL(clicked()), this, SLOT(selectNoneStudents()));

    this->createAppDirectories();
    this->clearAppDirectories();
    this->centerOnScreen();
    this->setTreeStyle();
    this->ui->btnPrint->setIcon(QIcon(":/images/greenprinter32.png"));
    this->ui->lblForgotenPassword->setText("<a href=\"http://kidsplaymath.org/moodle/login/forgot_password.php\">Forgotten your email or password?</a>");
    this->ui->lblForgotenPassword->setOpenExternalLinks(true);

    this->setupCourseCheckboxes();
    this->ui->btnNext_3->setEnabled(false);
    this->ui->btnPrint->setEnabled(false);
    this->resetProgressBar();
    this->setPageTitle(1, "Login");
    this->ui->stackedWidget->setCurrentIndex(1);

    this->setDownloadsEnabled(true);
    this->conversionsCount = 0;
    this->abortConversions = false;
}

void MainWindow::setupCourseCheckboxes(){
    this->ui->checkBoxHandouts->setChecked(false);
    this->ui->checkBoxAssignments->setChecked(true);
    this->ui->checkBoxForumPosts->setChecked(true);
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

    this->thread.cancel(); //Lo agregue porque sino había veces que esperaba eternamente el waitForFinish y la aplicación quedaba abierta
    this->thread.waitForFinished();
    this->sftp.setTransfersEnabled(true);
    this->setDownloadsEnabled(true);
}

void MainWindow::createAppDirectories(){
    Utils::createDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::HANDOUTS_LOCAL_PATH));
    Utils::createDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH));
    Utils::createDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::FORUM_POSTS_LOCAL_PATH));
}

void MainWindow::clearAppDirectories(){
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::HANDOUTS_LOCAL_PATH));
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH));
    Utils::clearDirectory(Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::FORUM_POSTS_LOCAL_PATH));
}

void MainWindow::setPageTitle(int step, QString task){
    int totalSteps = this->kpmteamLogin ? 4 : 3;
    this->ui->lblTask->setText(task);
    this->ui->lblSteps->setText("Step " + QString::number(step) + " of " + QString::number(totalSteps));
}

//Metodo encargado de llevar a cabo la conexion con la base de datos
bool MainWindow::connectToDatabase(){

    if(this->db.connect(Setting::Instance()->getValue(Setting::MYSQL_HOST_NAME), Setting::Instance()->getValue(Setting::MYSQL_DATABASE_NAME), Setting::Instance()->getValue(Setting::MYSQL_USERNAME), Setting::Instance()->getValue(Setting::MYSQL_PASSWORD))){
        return true;
    }
    else{
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
    int newHeight = geometry.height() - 99;
    this->setGeometry(x, y, width, newHeight);
    qApp->processEvents();
    this->centerOnScreen();
}

//Metodo para setear el estilo de los QTreeWidget
void MainWindow::setTreeStyle(){

    //Estilo del QTreeWidget para los Handouts y Assignments
    QStringList treeHeaders;
    treeHeaders << "File" << "Date" << "PrintData";
    this->ui->treeWidgetFiles->setColumnCount(3);
    this->ui->treeWidgetFiles->hideColumn(2);
    this->ui->treeWidgetFiles->setHeaderLabels(treeHeaders);
    this->ui->treeWidgetFiles->setColumnWidth(0, 320);
    this->ui->treeWidgetFiles->setColumnWidth(1, 66);
    this->ui->treeWidgetFiles->setIconSize(QSize(22, 22));

    //Estilo del QTreeWidget para la lista de students
    treeHeaders.clear();
    treeHeaders << "First Name" << "Last Name" << "Email" << "studentId";
    this->ui->treeWidgetStudents->setColumnCount(4);
    this->ui->treeWidgetStudents->hideColumn(3);
    this->ui->treeWidgetStudents->setHeaderLabels(treeHeaders);
    this->ui->treeWidgetStudents->setColumnWidth(1, 200);
    this->ui->treeWidgetStudents->setColumnWidth(2, 200);
    this->ui->treeWidgetStudents->setColumnWidth(3, 150);
}

void MainWindow::setTreeTopLevelItems(QString fileType){

    QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeWidgetFiles, QStringList() << fileType);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(0, Qt::Unchecked);
    item->setIcon(0, QIcon(":/images/folder.png"));
}

void MainWindow::updateCheckState(QTreeWidgetItem* item, int column){

    if(column != 0){
        return;
    }

    bool oldState = this->ui->treeWidgetFiles->blockSignals(true);

    if(!item->parent()){ //item es top-level
        this->updateChildrenCheckState(item);
    }
    else{
        this->updateParentCheckState(item);
    }
    this->updatePrintEnabledState();
    this->ui->treeWidgetFiles->blockSignals(oldState);
}

void MainWindow::updateChildrenCheckState(QTreeWidgetItem* item){

    if(item->checkState(0) == Qt::PartiallyChecked){
        return;
    }

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

void MainWindow::selectAllStudents(){
    QTreeWidgetItemIterator it(this->ui->treeWidgetStudents);
    while(*it){ //Recorro todos los estudiantes.
        (*it)->setCheckState(0, Qt::Checked);
        ++it;
    }
}

void MainWindow::selectNoneStudents(){
    QTreeWidgetItemIterator it(this->ui->treeWidgetStudents);
    while(*it){ //Recorro todos los estudiantes.
        (*it)->setCheckState(0, Qt::Unchecked);
        ++it;
    }
}

//Dependiendo de la eleccion del usuario descarga o no los handouts del servidor y avanza a la siguiente pantalla
void MainWindow::switchToLoginPage(){

    //Lleno el combo con los assignments que tienen por lo menos un assignment_submission online (data1 != '').
    this->db.getOnlineAssignments();
    QSqlQueryModel *onlineAssignments = this->db.getModel();
    for(int i=0; i<onlineAssignments->rowCount(); i++)
        this->ui->cmbAssignments->addItem(onlineAssignments->record(i).value("id").toString());

    this->setPageTitle(1, "Login");
    this->ui->stackedWidget->setCurrentIndex(1);
}

bool MainWindow::downloadCourseHandouts(int courseId){

    //QHostInfo hostInfo = QHostInfo::fromName(SFTP_HOST_NAME);
    QHostInfo hostInfo = QHostInfo::fromName(Setting::Instance()->getValue(Setting::SSH_HOST_NAME));

    if(hostInfo.addresses().isEmpty()){
        QMessageBox::critical(this, "Handouts downloading failed", "The program couldn't connect to the server "
                              "and the handouts couldn't be downloaded.");
        return false;
    }
    QString hostAddress = hostInfo.addresses().first().toString();

    //if(!this->sftp.open(hostAddress, SFTP_USERNAME, SFTP_PASSWORD)){
    if(!this->sftp.open(hostAddress, Setting::Instance()->getValue(Setting::SSH_USERNAME), Setting::Instance()->getValue(Setting::SSH_PASSWORD))){
        QMessageBox::critical(this, "Handouts downloading failed", "The program couldn't connect to the server "
                              "and the handouts couldn't be downloaded.");
        return false;
    }
    this->getHandoutsFileNames(courseId);
    thread = run(this, &MainWindow::downloadHandouts);
    return true;
}

void MainWindow::getHandoutsFileNames(int courseId){

    this->handoutsFileNames.clear();
    QString remoteFile;
    QString fileName;
    QSqlQueryModel *model;
    QString localPath = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::HANDOUTS_LOCAL_PATH);

    this->db.getCourseHandouts(courseId);
    model = this->db.getModel();
    //qDebug() << "DB Handout:" << model->rowCount();
    for (int i=0; i<model->rowCount(); i++){        
        remoteFile = Setting::Instance()->getValue(Setting::HANDOUTS_REMOTE_PATH) + "/" + Utils::fileHashToPath(model->record(i).value("contenthash").toString());
        fileName = localPath + "/" + model->record(i).value("filename").toString();
        this->handoutsFileNames.append(QPair<QString, QString>(remoteFile, fileName));
    }
    //qDebug() << "Handouts:" << this->handoutsFileNames;
}

//Descarga los handouts en background
void MainWindow::downloadHandouts(){

    QString remoteFile;
    QString localFile;

    for (int i = 0; i < this->handoutsFileNames.count(); i++){
        remoteFile = this->handoutsFileNames.at(i).first;
        localFile = this->handoutsFileNames.at(i).second;
        this->sftp.downloadFile(remoteFile, localFile);
        //qDebug() << "Downloaded file:" << localFile;
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

void MainWindow::loginAndSwitchPage(){

    qApp->processEvents();
    this->ui->btnNext_2->setEnabled(false);

    if(!connectToDatabase()){
        return;
    }

    QString email = this->ui->lineEditUsername->text();
    QString password = this->ui->lineEditPassword->text();
    this->ui->lblLoginFail->setStyleSheet("QLabel#lblLoginFail {color: #006EA8;}");
    this->ui->lblLoginFail->setText("Please wait while the application connects to the database...");
    qApp->processEvents();
    this->kpmteamLogin = false;

    if (email == Setting::Instance()->getValue(Setting::LOGIN_USERNAME_KPMTEAM) && password == Setting::Instance()->getValue(Setting::LOGIN_PASSWORD_KPMTEAM)){
        this->kpmteamLogin = true;
        fillCourses(); //Completa el combobox con todos los cursos disponibles
        this->studentNames.clear();
        this->filesToMergeByStudent.clear();
        this->setPageTitle(2, "Course and Student(s) selection");        
        this->ui->stackedWidget->setCurrentIndex(5);
        this->enlargeWindow();
    }
    else{
        if(!this->db.userLogin(email, password)){
            this->ui->lblLoginFail->setStyleSheet("QLabel#lblLoginFail {color: red;}");
            this->ui->lblLoginFail->setText("Your email or password is incorrect. Please try again.");
            this->ui->btnNext_2->setEnabled(true);
            return;
        }
        //Seteo el nombre del archivo de salida ahora que el modelo tiene los datos del usuario.
        QSqlRecord userRecord = this->db.getModel()->record(0);
        QString userId = userRecord.value("id").toString();
        QString outputFile = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/KpmPortfolio " +
                userRecord.value("firstname").toString() + userRecord.value("lastname").toString()[0].toUpper() + ".pdf";
        this->pdfmerge.setOutputFileName(outputFile);        

        //Se asegura el acceso al server antes de obtener handouts y lanzar el hilo
        this->db.getUserCourse(userId.toInt());
        //int courseId = this->db.getModel()->record(0).value("enrolid").toInt();
        int courseId = this->db.getModel()->record(0).value("courseid").toInt();
        this->downloadCourseHandouts(courseId);

        this->fillTreeFromUser(userId.toInt());
        this->setPageTitle(2, "File selection");
        this->ui->stackedWidget->setCurrentIndex(3);
        this->enlargeWindow();
    }
}

void MainWindow::fillCourses(){

    this->ui->cmbCourses->clear();
    this->db.getAllCourses();
    QSqlQueryModel *coursesModel = this->db.getModel();

    if (coursesModel->rowCount() != 0){
        this->hashCourses.clear();

        for (int i = 0; i < coursesModel->rowCount(); i++){
            this->hashCourses[i] = coursesModel->record(i).value("id").toInt();
            this->ui->cmbCourses->addItem(coursesModel->record(i).value("shortname").toString());
        }
        QObject::connect(this->ui->cmbCourses, SIGNAL(currentIndexChanged(int)), this, SLOT(fillStudentsFromCourse()));
        fillStudentsFromCourse();
    }
}

void MainWindow::fillStudentsFromCourse(){

    this->ui->treeWidgetStudents->clear();    
    this->db.getStudentsByCourse(this->hashCourses[this->ui->cmbCourses->currentIndex()]);
    QSqlQueryModel *studentsModel = this->db.getModel();

    if (studentsModel->rowCount() != 0){
        QStringList itemData;

        for (int i = 0; i < studentsModel->rowCount(); i++){
            itemData << studentsModel->record(i).value("firstname").toString() <<
                        studentsModel->record(i).value("lastname").toString() <<
                        studentsModel->record(i).value("email").toString() <<
                        studentsModel->record(i).value("id").toString();
            QTreeWidgetItem *studentItem = new QTreeWidgetItem(itemData);
            studentItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            studentItem->setCheckState(0, Qt::Checked);
            this->ui->treeWidgetStudents->addTopLevelItem(studentItem);
            itemData.clear();
        }
    }
}

void MainWindow::switchToCategoriesPage(){

    QTreeWidgetItemIterator it(this->ui->treeWidgetStudents, QTreeWidgetItemIterator::Checked);
    while(*it){ //Recorro estudiantes seleccionados para llenar el hash de nombres.
        qDebug() << "Student:" << (*it)->text(3) << (*it)->text(0) << (*it)->text(1);
        qDebug() << (*it)->text(3).toInt();
        this->studentNames[(*it)->text(3).toInt()] = (*it)->text(0) + " " + (*it)->text(1);
        ++it;
    }

    if (this->studentNames.count() > 0){
        this->setPageTitle(3, "File selection");
        this->ui->stackedWidget->setCurrentIndex(6);
        this->reduceWindow();
    }
    else QMessageBox::critical(this, "Students selection", "You must select at least one student.");
}

void MainWindow::switchToProgressPageFromCourse(){

    QSqlQueryModel *model;
    bool switchedMade = false;
    this->printingEnabled = false;

    if(!(this->ui->checkBoxHandouts->isChecked() || this->ui->checkBoxAssignments->isChecked() || this->ui->checkBoxForumPosts->isChecked())){
        QMessageBox::critical(this, "File selection", "You must select at least one category.");
        return;
    }

    if(this->ui->checkBoxHandouts->isChecked()){
        qDebug() << "Descargando los handouts [caso kpmteam]";
        int courseId = this->hashCourses[this->ui->cmbCourses->currentIndex()];//HABLARLO CON CARLOS (antes se sumaba 1)
        if(this->downloadCourseHandouts(courseId)){
            this->ui->progressBar->setRange(0, this->handoutsFileNames.count()); //Ya tengo los nombres, la descarga se ejecuta en otro hilo.
            if(this->handoutsFileNames.count() > 0){
                this->switchPage();
                switchedMade = true;
            }
        }
    }
    if(!this->abortConversions && this->ui->checkBoxAssignments->isChecked()){
        this->convertCourseAssignments();
        model = this->db.getModel();
        if(!switchedMade && model->rowCount() > 0){
            this->switchPage();
            switchedMade = true;
        }
    }

    if(!this->abortConversions && this->ui->checkBoxForumPosts->isChecked()){
        this->convertCourseForumPosts();
        model = this->db.getModel();
        if(!switchedMade && model->rowCount() > 0){
            this->switchPage();
            switchedMade = true;
        }
    }

    this->printingEnabled = true; //Habilita el botón Create Portfolio
    this->checkProgressBar();

    if(!switchedMade){
        QMessageBox::critical(this, "File selection", "There are no files for the selected students and categories.");
    }
}

void MainWindow::switchPage(){
    this->setPageTitle(4, "Print");
    this->ui->stackedWidget->setCurrentIndex(4);
    this->reduceWindow();
}

void MainWindow::convertCourseAssignments(){

    int studentId;
    QString localFile, html;
    int courseId = this->hashCourses[this->ui->cmbCourses->currentIndex()];//Hablar con Carlos. Antes se sumaba + 1;
    this->db.getOnlineFilesByUsers(courseId, this->studentNames.keys());
    QSqlQueryModel *model = this->db.getModel(); //Los assignments de todos los estudiantes juntos.
    qDebug() << "Assignments count:" << model->rowCount();
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + model->rowCount());

    for (int i=0; i<model->rowCount(); i++){ //Para cada uno, lo convierto y lo guardo en la lista 'toMerge' del alumno correspondiente.
        this->conversionsLock.lockForWrite();
        studentId = model->record(i).value("userId").toInt();
        qDebug() << "Assignment userid:" << studentId;        
        localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH) + "/" + model->record(i).value("name").toString() + ".pdf";
        html = Utils::dataToHtml(model->record(i).value("name").toString(),
                                    model->record(i).value("intro").toString(),
                                    model->record(i).value("data1").toString());
        this->pdfmerge.htmlToPdf(html, localFile);
        this->filesToMergeByStudent[studentId] << QPair<QString, int>(localFile, MainWindow::ASSIGNMENT);
        emit this->downloadedFile();
        this->conversionsCount++;
        this->conversionsLock.unlock();
        if(this->abortConversions){
            break;
        }
    }
}

void MainWindow::convertCourseForumPosts(){

    int studentId;
    QString localFile, html;
    this->db.getForumPostsByUsers(this->studentNames.keys());
    QSqlQueryModel *model = this->db.getModel(); //Los forum posts de todos los estudiantes juntos.
    qDebug() << "Forum posts count:" << model->rowCount();
    this->ui->progressBar->setRange(0, this->ui->progressBar->maximum() + model->rowCount());

    for (int i=0; i<model->rowCount(); i++){ //Para cada uno, lo convierto y lo guardo en la lista 'toMerge' del alumno correspondiente.
        this->conversionsLock.lockForWrite();
        studentId = model->record(i).value("userId").toInt();
        qDebug() << "Forum post userid:" << studentId;        
        localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::FORUM_POSTS_LOCAL_PATH) + "/" +
                    model->record(i).value("pregSubject").toString().remove("Re: ", Qt::CaseInsensitive) + " " +
                    model->record(i).value("respId").toString() + ".pdf";
        html = Utils::dataToHtml(model->record(i).value("pregSubject").toString(),
                                    model->record(i).value("pregMessage").toString(),
                                    model->record(i).value("respMessage").toString());
        this->pdfmerge.htmlToPdf(html, localFile);
        this->filesToMergeByStudent[studentId] << QPair<QString, int>(localFile, MainWindow::FORUM_POST);
        emit this->downloadedFile();
        this->conversionsCount++;
        this->conversionsLock.unlock();
        if(this->abortConversions){
            break;
        }
    }
}

void MainWindow::switchToTreePageFromAssignment(){

    this->fillTreeFromAssignment();
    this->setPageTitle(2, "Assignment selection");
    this->ui->stackedWidget->setCurrentIndex(2);
    this->enlargeWindow();
}

void MainWindow::insertOrderedTreeItem(QTreeWidgetItem *parentItem, QTreeWidgetItem *item){

    int i=0;
    for(i; i<parentItem->childCount(); i++){
        if(Utils::customSort(QPair<QString, int>(item->text(0), 0), QPair<QString, int>(parentItem->child(i)->text(0), 0)))
            break;
    }
    parentItem->insertChild(i, item);
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
            itemData << QString(this->handoutsFileNames.at(i).second).section('/', -1).replace(".pdf", "");
            QTreeWidgetItem *item = new QTreeWidgetItem(itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/pdf_file.png"));
            this->insertOrderedTreeItem(handouts, item);
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

            html = Utils::dataToHtml(onlineFilesModel->record(i).value("name").toString(),
                                        onlineFilesModel->record(i).value("intro").toString(),
                                        onlineFilesModel->record(i).value("data1").toString());

            itemData << name << Utils::timeStampToDate(onlineFilesModel->record(i).value(5).toInt()) << html;
            QTreeWidgetItem *item = new QTreeWidgetItem(itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/html_file.png"));
            this->insertOrderedTreeItem(onlineAssignments, item);
            itemData.clear();
        }
    }

    this->db.getForumPostsByUser(userId);
    QSqlQueryModel *forumPostsModel = this->db.getModel();

    if (forumPostsModel->rowCount() != 0){

        setTreeTopLevelItems("Forum Posts");
        QTreeWidgetItem *forumPosts = this->getFileTypeItem("Forum Posts");
        forumPosts->setCheckState(0, Qt::Checked);

        for (i=0; i<forumPostsModel->rowCount(); i++){
            name = forumPostsModel->record(i).value("pregSubject").toString().remove("Re: ", Qt::CaseInsensitive);
            count = this->getTreeNameCount(name);
            if(count > 0)
                name += " [" + QString::number(count + 1) + "]";
            html = Utils::dataToHtml(forumPostsModel->record(i).value("pregSubject").toString(),
                                     forumPostsModel->record(i).value("pregMessage").toString(),
                                     forumPostsModel->record(i).value("respMessage").toString());

            itemData << name << Utils::timeStampToDate(forumPostsModel->record(i).value("respModified").toInt()) << html;
            QTreeWidgetItem *item = new QTreeWidgetItem(itemData);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
            item->setIcon(0, QIcon(":/images/forum_file.png"));
            this->insertOrderedTreeItem(forumPosts, item);
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
    this->printingEnabled = false;
    int countChecked = 0;//sin tener en cuenta los handouts

    QTreeWidgetItemIterator it(this->ui->treeWidgetFiles, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while (*it){
        if((*it)->parent()->text(0) != "Handouts"){            
            countChecked++;
        }
        ++it;
    }

    //Se actualiza la cantidad de archivos a descargar y/o convertir
    this->printingEnabled = true;
    this->ui->progressBar->setRange(0, this->handoutsFileNames.count() + countChecked);
    this->checkProgressBar();

    this->setPageTitle(3, "Print");
    this->ui->stackedWidget->setCurrentIndex(4);
    this->reduceWindow();

    this->filesToMerge.clear();
    this->setHandoutsToMerge();

    if(!this->abortConversions){
        this->convertOnlineFiles();
    }

    if(!this->abortConversions){
        this->convertForumPostsFiles();
    }    
}

//Actualiza la progress bar a medida que se van descargando los archivos y que se van convirtiendo a pdf los assignment online
void MainWindow::updateProgressBar(){
    //qDebug() << "Update Progress Bar:" << this->ui->progressBar->value() + 1;
    this->ui->progressBar->setValue(this->ui->progressBar->value() + 1);    
}

void MainWindow::checkProgressBar(){
    qDebug() << "Check Progress Bar:" << this->ui->progressBar->value() << this->ui->progressBar->maximum();
    this->ui->btnPrint->setEnabled(this->printingEnabled && (this->ui->progressBar->value() == this->ui->progressBar->maximum()));
}

void MainWindow::resetProgressBar(){
    this->ui->progressBar->setRange(0, 0);
    this->ui->progressBar->setValue(0);
}

//Descarga los assignment de tipo upload del servidor
void MainWindow::downloadUploadFiles(){

    //QHostInfo hostInfo = QHostInfo::fromName(SFTP_HOST_NAME);
    QHostInfo hostInfo = QHostInfo::fromName(Setting::Instance()->getValue(Setting::SSH_HOST_NAME));
    QString serverAddress = hostInfo.addresses().first().toString();

    Sftp sftp2;
    //sftp2.open(serverAddress, SFTP_USERNAME, SFTP_PASSWORD);
    sftp2.open(serverAddress, Setting::Instance()->getValue(Setting::SSH_USERNAME), Setting::Instance()->getValue(Setting::SSH_PASSWORD));
    QString localFile;
    int index = 0;
    QTreeWidgetItemIterator it(this->getFileTypeItem("Assignments"), QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);

    while (*it){
        if ((*it)->text(0).contains("(Document)")){            
            localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH) + "/"  + QString::number(index) + ". " +
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

    if (handoutsItem == NULL){
        return;
    }

    QString localFile;
    QTreeWidgetItemIterator it(handoutsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != handoutsItem){
            break;
        }

        localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::HANDOUTS_LOCAL_PATH) + "/"  + (*it)->text(0) + ".pdf";
        this->filesToMerge << QPair<QString, int>(localFile, MainWindow::HANDOUT);
        ++it;
    }
}

//Convierte a pdf el contenido de los assignment de tipo online de la tabla
void MainWindow::convertOnlineFiles(){    

    QTreeWidgetItem* assignmentsItem = this->getFileTypeItem("Assignments");

    if (assignmentsItem == NULL){
        return;
    }

    QString localFile;
    QTreeWidgetItemIterator it(assignmentsItem, QTreeWidgetItemIterator::Checked | QTreeWidgetItemIterator::NoChildren);
    while(*it){
        if((*it)->parent() != assignmentsItem){
            break;
        }

        if ((*it)->text(0).contains("(Text)")){
            this->conversionsLock.lockForWrite();            
            localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH) + "/" + (*it)->text(0).replace(" (Text)", "") + ".pdf";
            this->pdfmerge.htmlToPdf((*it)->text(2), localFile);
            this->filesToMerge << QPair<QString, int>(localFile, MainWindow::ASSIGNMENT);
            emit this->downloadedFile();
            this->conversionsCount++;
            this->conversionsLock.unlock();
            if(this->abortConversions){
                break;
            }
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
        if((*it)->parent() != forumPostsItem){
            break;
        }

        this->conversionsLock.lockForWrite();        
        localFile = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::FORUM_POSTS_LOCAL_PATH) + "/" + (*it)->text(0).replace(":", "") + ".pdf";
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

void MainWindow::mergeFiles(QList<QPair<QString, int> > files, QString studentName){

    qSort(files.begin(), files.end(), Utils::customSort);
    QPair<QString, int> file;

    foreach(file, files){
        QString errorString;
        if (file.second == MainWindow::HANDOUT){//Si es un handout, como estos pertenecen al curso no hace falta ponerle a las hojas
                                                //el nombre del estudiante.
            this->pdfmerge.addPdf(file.first, "", errorString);
        }
        else {
            this->pdfmerge.addPdf(file.first, studentName, errorString);
        }
    }
}

void MainWindow::mergeAndPrint(){

    this->pdfmerge.setupDocument();

    if(!this->kpmteamLogin){ //Un sólo archivo.       
        this->mergeFiles(this->filesToMerge);
        if(this->pdfmerge.writeOutput()){
            QMessageBox::information(this, "Successful printing", "Your portfolio has been created and has been saved to your desktop. You can now print it.");
            QDesktopServices::openUrl(QUrl("file:///" + this->pdfmerge.getOutputFileName()));
        }else{
            QMessageBox::critical(this, "Printing failed", "The program couldn't save the output file.");
        }
    }else{ //Múltiples archivos.
        bool successfulPrinting = true;
        QList<QPair<QString, int> > handoutsToMerge = this->getHandoutsToMerge();

        if(this->studentNames.count() < 5){
            QString failedReports = "";

            foreach (int i, this->studentNames.keys()){                
                if (!this->filesToMergeByStudent.contains(i) && this->ui->checkBoxAssignments->isChecked() && this->ui->checkBoxForumPosts->isChecked()){//El student no tiene ni assignments ni forum post
                    this->mergeFiles(handoutsToMerge, this->studentNames[i]);
                    QString tmpPath = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH);
                    this->pdfmerge.addPageNeitherAssignmentsNorForumPost(tmpPath, this->studentNames[i]);
                    QDir dir(tmpPath);
                    dir.remove("NoAssigmentNoForumPost.pdf");//Archivo temporal utilizado para poner una pagina que dice
                                                             //que el usuario no tiene ni assignments ni forum post.
                }
                else{
                    this->mergeFiles(handoutsToMerge + this->filesToMergeByStudent[i]);
                }

                QString outputFile = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/KpmPortfolio " +
                        //this->studentNames[i.key()].toUpper() + ".pdf";
                        this->studentNames[i].toUpper() + ".pdf";

                this->pdfmerge.setOutputFileName(outputFile);
                if(this->pdfmerge.writeOutput()){
                    QDesktopServices::openUrl(QUrl("file:///" + this->pdfmerge.getOutputFileName()));
                }
                else {
                    successfulPrinting = false;
                    if (failedReports == ""){
                        //failedReports = this->studentNames[i.key()].toUpper();
                        failedReports = this->studentNames[i].toUpper();
                    }
                    else {
                        //failedReports = failedReports + ", " + this->studentNames[i.key()].toUpper();
                        failedReports = failedReports + ", " + this->studentNames[i].toUpper();
                    }
                }

                this->pdfmerge.setupDocument();
            }

            if(successfulPrinting){
                QMessageBox::information(this, "Successful printing", "The portfolios have been created and have been saved to your desktop. You can now print them.");
            }else{
                QMessageBox::critical(this, "Printing failed", "The program had problems to create some of the portfolios: " + failedReports);
            }
        }
        else{
            if (handoutsToMerge.count() > 0){
                this->mergeFiles(handoutsToMerge);            
            }

            QString outputFile = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/KpmPortfolio Course " +
                    this->ui->cmbCourses->currentText().toUpper() + ".pdf";
            this->pdfmerge.setOutputFileName(outputFile);

            foreach (int i, this->studentNames.keys()){
                if (this->filesToMergeByStudent.contains(i)){//Significa que tiene assignments y/o forum post
                    this->mergeFiles(this->filesToMergeByStudent[i], this->studentNames[i]);
                    this->pdfmerge.addPageSeparator();
                }
                else{
                    if (this->ui->checkBoxAssignments->isChecked() && this->ui->checkBoxForumPosts->isChecked()){//El student no tiene ni assignments ni forum post
                        QString tmpPath = Utils::getUserDirectory() + "/" + Setting::Instance()->getValue(Setting::ASSIGNMENTS_LOCAL_PATH);
                        this->pdfmerge.addPageNeitherAssignmentsNorForumPost(tmpPath, this->studentNames[i]);
                        QDir dir(tmpPath);
                        dir.remove("NoAssigmentNoForumPost.pdf");//Archivo temporal utilizado para poner una pagina que dice
                                                                 //que el usuario no tiene ni assignments ni forum post.
                    }
                }
            }

            if(this->pdfmerge.writeOutput()){
                QMessageBox::information(this, "Successful printing", "Your portfolio has been created and has been saved to your desktop. You can now print it.");
                QDesktopServices::openUrl(QUrl("file:///" + this->pdfmerge.getOutputFileName()));
            }else{
                QMessageBox::critical(this, "Printing failed", "The program couldn't save the output file.");
            }
        }
    }
}

//Retorna la lista de handouts en el formato necesario para mergear con los demás archivos.
QList<QPair<QString, int> > MainWindow::getHandoutsToMerge(){
    QPair<QString, QString> handout;
    QList<QPair<QString, int> > result;

    foreach(handout, this->handoutsFileNames){
        result.append(QPair<QString, int>(handout.second, MainWindow::HANDOUT));
    }
    return result;
}

void MainWindow::backToLoginPage(){

    disconnect(this->ui->cmbCourses, SIGNAL(currentIndexChanged(int)), this, SLOT(fillStudentsFromCourse()));
    this->finishDownloadThread();
    qDebug() << "Back to login: downloads finished.";

    this->setPageTitle(1, "Login");
    this->ui->lineEditUsername->setText("");
    this->ui->lineEditPassword->setText("");
    this->ui->lblLoginFail->setText("");
    this->ui->btnNext_2->setEnabled(true);
    this->ui->stackedWidget->setCurrentIndex(1);
    this->reduceWindow();
    this->ui->treeWidgetFiles->clear();
    this->ui->progressBar->setValue(0);
    this->clearAppDirectories();
}

void MainWindow::backToTreePageFromUser(){
    this->abortConversions = true;
    this->conversionsLock.lockForRead();
    //qDebug() << "Conversions:" << this->conversionsCount;
    if(this->kpmteamLogin){
        this->resetProgressBar();
    }
    else{
        this->ui->progressBar->setValue(this->ui->progressBar->value() - this->conversionsCount);
    }

    this->conversionsLock.unlock();
    this->abortConversions = false;
    this->conversionsCount = 0;

    if(this->kpmteamLogin){
        this->finishDownloadThread();
        this->handoutsFileNames.clear();
        this->filesToMergeByStudent.clear();
        this->resetProgressBar();
        this->setPageTitle(3, "File selection");
        this->ui->stackedWidget->setCurrentIndex(6);
    }
    else{
        this->setPageTitle(2, "File selection");
        this->ui->stackedWidget->setCurrentIndex(3);
        this->enlargeWindow();
    }
}

void MainWindow::backToCoursesPage(){

    this->studentNames.clear();
    this->setPageTitle(2, "Course and Student(s) selection");
    this->ui->stackedWidget->setCurrentIndex(5);
    this->resetProgressBar();
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

MainWindow::~MainWindow(){
    delete ui;
}
