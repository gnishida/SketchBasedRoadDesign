/********************************************************************************
** Form generated from reading UI file 'MyMainWindow.ui'
**
** Created: Sun Dec 15 22:22:43 2013
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
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyMainWindowClass
{
public:
    QAction *actionControlWidget;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionReferenceRoads;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuWindow;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MyMainWindowClass)
    {
        if (MyMainWindowClass->objectName().isEmpty())
            MyMainWindowClass->setObjectName(QString::fromUtf8("MyMainWindowClass"));
        MyMainWindowClass->resize(1090, 743);
        actionControlWidget = new QAction(MyMainWindowClass);
        actionControlWidget->setObjectName(QString::fromUtf8("actionControlWidget"));
        actionNew = new QAction(MyMainWindowClass);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionOpen = new QAction(MyMainWindowClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExit = new QAction(MyMainWindowClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionReferenceRoads = new QAction(MyMainWindowClass);
        actionReferenceRoads->setObjectName(QString::fromUtf8("actionReferenceRoads"));
        centralWidget = new QWidget(MyMainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MyMainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MyMainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1090, 21));
        menuWindow = new QMenu(menuBar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MyMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MyMainWindowClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MyMainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MyMainWindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MyMainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuWindow->menuAction());
        menuWindow->addAction(actionControlWidget);
        menuWindow->addAction(actionReferenceRoads);
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);

        retranslateUi(MyMainWindowClass);

        QMetaObject::connectSlotsByName(MyMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MyMainWindowClass)
    {
        MyMainWindowClass->setWindowTitle(QApplication::translate("MyMainWindowClass", "MyMainWindow", 0, QApplication::UnicodeUTF8));
        actionControlWidget->setText(QApplication::translate("MyMainWindowClass", "ControlWidget", 0, QApplication::UnicodeUTF8));
        actionNew->setText(QApplication::translate("MyMainWindowClass", "New", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MyMainWindowClass", "Open", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MyMainWindowClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionReferenceRoads->setText(QApplication::translate("MyMainWindowClass", "Reference Roads", 0, QApplication::UnicodeUTF8));
        menuWindow->setTitle(QApplication::translate("MyMainWindowClass", "Window", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MyMainWindowClass", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MyMainWindowClass: public Ui_MyMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYMAINWINDOW_H
