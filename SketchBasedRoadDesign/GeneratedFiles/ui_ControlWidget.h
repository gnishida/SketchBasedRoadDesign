/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Fri Dec 6 18:57:22 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QPushButton *pushButtonSearch;
    QPushButton *pushButtonOK;
    QSlider *horizontalSlider;
    QPushButton *pushButtonSave;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(148, 261);
        ControlWidget->setMinimumSize(QSize(145, 261));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        pushButtonSearch = new QPushButton(dockWidgetContents);
        pushButtonSearch->setObjectName(QString::fromUtf8("pushButtonSearch"));
        pushButtonSearch->setGeometry(QRect(10, 10, 131, 31));
        pushButtonOK = new QPushButton(dockWidgetContents);
        pushButtonOK->setObjectName(QString::fromUtf8("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(10, 70, 131, 31));
        horizontalSlider = new QSlider(dockWidgetContents);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(10, 120, 131, 20));
        horizontalSlider->setOrientation(Qt::Horizontal);
        pushButtonSave = new QPushButton(dockWidgetContents);
        pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));
        pushButtonSave->setGeometry(QRect(10, 200, 131, 31));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        pushButtonSearch->setText(QApplication::translate("ControlWidget", "Search", 0, QApplication::UnicodeUTF8));
        pushButtonOK->setText(QApplication::translate("ControlWidget", "OK", 0, QApplication::UnicodeUTF8));
        pushButtonSave->setText(QApplication::translate("ControlWidget", "Save", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
