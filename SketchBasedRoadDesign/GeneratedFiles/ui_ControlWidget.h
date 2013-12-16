/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Mon Dec 16 10:28:46 2013
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
#include <QtGui/QLabel>
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
    QPushButton *pushButtonClear;
    QPushButton *pushButtonModeSketch;
    QPushButton *pushButtonModeView;
    QPushButton *pushButtonModeSelect;
    QLabel *label;
    QSlider *horizontalSliderZoom;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(189, 661);
        ControlWidget->setMinimumSize(QSize(189, 660));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        pushButtonSearch = new QPushButton(dockWidgetContents);
        pushButtonSearch->setObjectName(QString::fromUtf8("pushButtonSearch"));
        pushButtonSearch->setGeometry(QRect(20, 130, 151, 31));
        pushButtonOK = new QPushButton(dockWidgetContents);
        pushButtonOK->setObjectName(QString::fromUtf8("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(20, 250, 151, 31));
        horizontalSlider = new QSlider(dockWidgetContents);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(20, 300, 151, 20));
        horizontalSlider->setOrientation(Qt::Horizontal);
        pushButtonSave = new QPushButton(dockWidgetContents);
        pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));
        pushButtonSave->setGeometry(QRect(20, 380, 151, 31));
        pushButtonClear = new QPushButton(dockWidgetContents);
        pushButtonClear->setObjectName(QString::fromUtf8("pushButtonClear"));
        pushButtonClear->setGeometry(QRect(20, 170, 151, 31));
        pushButtonModeSketch = new QPushButton(dockWidgetContents);
        pushButtonModeSketch->setObjectName(QString::fromUtf8("pushButtonModeSketch"));
        pushButtonModeSketch->setGeometry(QRect(70, 10, 51, 51));
        pushButtonModeView = new QPushButton(dockWidgetContents);
        pushButtonModeView->setObjectName(QString::fromUtf8("pushButtonModeView"));
        pushButtonModeView->setGeometry(QRect(10, 10, 51, 51));
        pushButtonModeSelect = new QPushButton(dockWidgetContents);
        pushButtonModeSelect->setObjectName(QString::fromUtf8("pushButtonModeSelect"));
        pushButtonModeSelect->setGeometry(QRect(130, 10, 51, 51));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 80, 46, 16));
        horizontalSliderZoom = new QSlider(dockWidgetContents);
        horizontalSliderZoom->setObjectName(QString::fromUtf8("horizontalSliderZoom"));
        horizontalSliderZoom->setGeometry(QRect(50, 80, 131, 20));
        horizontalSliderZoom->setOrientation(Qt::Horizontal);
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        pushButtonSearch->setText(QApplication::translate("ControlWidget", "Search", 0, QApplication::UnicodeUTF8));
        pushButtonOK->setText(QApplication::translate("ControlWidget", "OK", 0, QApplication::UnicodeUTF8));
        pushButtonSave->setText(QApplication::translate("ControlWidget", "Save", 0, QApplication::UnicodeUTF8));
        pushButtonClear->setText(QApplication::translate("ControlWidget", "Clear", 0, QApplication::UnicodeUTF8));
        pushButtonModeSketch->setText(QApplication::translate("ControlWidget", "Sketch", 0, QApplication::UnicodeUTF8));
        pushButtonModeView->setText(QApplication::translate("ControlWidget", "View", 0, QApplication::UnicodeUTF8));
        pushButtonModeSelect->setText(QApplication::translate("ControlWidget", "Select", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ControlWidget", "Zoom:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
