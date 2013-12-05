/********************************************************************************
** Form generated from reading UI file 'MyMainWindow.ui'
**
** Created: Thu Dec 5 14:15:42 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYMAINWINDOW_H
#define UI_MYMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyMainWindowClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MyMainWindowClass)
    {
        if (MyMainWindowClass->objectName().isEmpty())
            MyMainWindowClass->setObjectName(QString::fromUtf8("MyMainWindowClass"));
        MyMainWindowClass->resize(1090, 743);
        centralWidget = new QWidget(MyMainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MyMainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MyMainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1090, 21));
        MyMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MyMainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MyMainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MyMainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MyMainWindowClass->setStatusBar(statusBar);

        retranslateUi(MyMainWindowClass);

        QMetaObject::connectSlotsByName(MyMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MyMainWindowClass)
    {
        MyMainWindowClass->setWindowTitle(QApplication::translate("MyMainWindowClass", "MyMainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MyMainWindowClass: public Ui_MyMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYMAINWINDOW_H
