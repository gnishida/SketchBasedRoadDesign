#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MyMainWindow.h"
#include "MyGraphicsView.h"
#include "ControlWidget.h"
#include <qscrollarea.h>

class MyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Ui::MyMainWindowClass ui;
	QScrollArea* scrollArea;
	MyGraphicsView* view;
	ControlWidget* controlWidget;

public:
	MyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyMainWindow();

private slots:
	void showControlWidget();
};

#endif // MYMAINWINDOW_H
