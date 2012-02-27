/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Feb 27 18:18:22 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QPushButton *btnAccept;
    QPushButton *btnCancel;
    QLabel *label;
    QWidget *page_2;
    QGroupBox *groupBox;
    QLineEdit *lineEditLastName;
    QLineEdit *lineEditUserName;
    QLineEdit *lineEditNickname;
    QLabel *lblNickname;
    QLabel *lblUserName;
    QLabel *lblLastName;
    QPushButton *btnCancel_2;
    QPushButton *btnNext;
    QWidget *page_3;
    QTableWidget *tableWidgetAssignments;
    QPushButton *btnCancel_3;
    QPushButton *btnNext_2;
    QWidget *page_4;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(734, 486);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        btnAccept = new QPushButton(page);
        btnAccept->setObjectName(QString::fromUtf8("btnAccept"));
        btnAccept->setGeometry(QRect(540, 370, 78, 25));
        btnCancel = new QPushButton(page);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        btnCancel->setGeometry(QRect(630, 370, 78, 25));
        label = new QLabel(page);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(120, 120, 461, 91));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        groupBox = new QGroupBox(page_2);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(170, 120, 391, 121));
        lineEditLastName = new QLineEdit(groupBox);
        lineEditLastName->setObjectName(QString::fromUtf8("lineEditLastName"));
        lineEditLastName->setGeometry(QRect(100, 80, 281, 21));
        lineEditUserName = new QLineEdit(groupBox);
        lineEditUserName->setObjectName(QString::fromUtf8("lineEditUserName"));
        lineEditUserName->setGeometry(QRect(100, 50, 281, 21));
        lineEditNickname = new QLineEdit(groupBox);
        lineEditNickname->setObjectName(QString::fromUtf8("lineEditNickname"));
        lineEditNickname->setGeometry(QRect(100, 20, 281, 21));
        lblNickname = new QLabel(groupBox);
        lblNickname->setObjectName(QString::fromUtf8("lblNickname"));
        lblNickname->setGeometry(QRect(30, 20, 56, 15));
        lblUserName = new QLabel(groupBox);
        lblUserName->setObjectName(QString::fromUtf8("lblUserName"));
        lblUserName->setGeometry(QRect(33, 53, 56, 15));
        lblLastName = new QLabel(groupBox);
        lblLastName->setObjectName(QString::fromUtf8("lblLastName"));
        lblLastName->setGeometry(QRect(32, 81, 56, 15));
        btnCancel_2 = new QPushButton(page_2);
        btnCancel_2->setObjectName(QString::fromUtf8("btnCancel_2"));
        btnCancel_2->setGeometry(QRect(630, 370, 78, 25));
        btnNext = new QPushButton(page_2);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));
        btnNext->setGeometry(QRect(540, 370, 78, 25));
        stackedWidget->addWidget(page_2);
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        tableWidgetAssignments = new QTableWidget(page_3);
        tableWidgetAssignments->setObjectName(QString::fromUtf8("tableWidgetAssignments"));
        tableWidgetAssignments->setGeometry(QRect(10, 10, 691, 351));
        btnCancel_3 = new QPushButton(page_3);
        btnCancel_3->setObjectName(QString::fromUtf8("btnCancel_3"));
        btnCancel_3->setGeometry(QRect(630, 370, 78, 25));
        btnNext_2 = new QPushButton(page_3);
        btnNext_2->setObjectName(QString::fromUtf8("btnNext_2"));
        btnNext_2->setGeometry(QRect(540, 370, 78, 25));
        stackedWidget->addWidget(page_3);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        stackedWidget->addWidget(page_4);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 734, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        btnAccept->setText(QApplication::translate("MainWindow", "&Accept", 0, QApplication::UnicodeUTF8));
        btnCancel->setText(QApplication::translate("MainWindow", "&Cancel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Ac\303\241 la pregunta sobre si quiere imprimir los handouts", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        lblNickname->setText(QApplication::translate("MainWindow", "Nickname", 0, QApplication::UnicodeUTF8));
        lblUserName->setText(QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
        lblLastName->setText(QApplication::translate("MainWindow", "Last name", 0, QApplication::UnicodeUTF8));
        btnCancel_2->setText(QApplication::translate("MainWindow", "&Cancel", 0, QApplication::UnicodeUTF8));
        btnNext->setText(QApplication::translate("MainWindow", "&Next", 0, QApplication::UnicodeUTF8));
        btnCancel_3->setText(QApplication::translate("MainWindow", "&Cancel", 0, QApplication::UnicodeUTF8));
        btnNext_2->setText(QApplication::translate("MainWindow", "&Next", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
