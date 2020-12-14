/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QTabWidget *tabwidget;
    QWidget *tab_basic;
    QWidget *tab_advanced;
    QWidget *tab_predict;
    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 750);
        //centralwidget = new QWidget(MainWindow);
        centralwidget = new QWidget();
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        tabwidget=new QTabWidget(centralwidget);
        tabwidget->setTabPosition(QTabWidget::North);
        tabwidget->setUsesScrollButtons(false);
        tabwidget->setTabsClosable(false);
        tabwidget->setDocumentMode(false);
        tabwidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        tab_basic=new QWidget();
        tab_advanced=new QWidget();
        tab_predict=new QWidget();
        tabwidget->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        tabwidget->addTab(tab_basic,"Basic Visualization");
        tabwidget->addTab(tab_advanced,"Free Query");
        tabwidget->addTab(tab_predict,"Advanced Visualization");
        //QGridLayout *gridlayout=new QGridLayout();
        //gridlayout->addWidget(centralwidget,0,0,1,1);
        //MainWindow->setLayout(gridlayout);
        centralwidget->resize(MainWindow->size());
        tabwidget->resize(centralwidget->size());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
