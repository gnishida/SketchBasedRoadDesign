#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MyMainWindow.h"
#include "MyGraphicsView.h"
#include "RoadDBView.h"
#include "ControlWidget.h"
#include <qscrollarea.h>

class MyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Ui::MyMainWindowClass ui;
	QScrollArea* scrollArea;
	MyGraphicsView* view;
	std::vector<RoadDBView*> examples;
	ControlWidget* controlWidget;

public:
	MyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyMainWindow();

private slots:
	void showControlWidget();
};

#endif // MYMAINWINDOW_H
