#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MyMainWindow.h"
#include "GLWidget.h"
#include "RoadBoxList.h"
#include "ControlWidget.h"
#include <qscrollarea.h>

class MyMainWindow : public QMainWindow
{
Q_OBJECT

public:
	static enum { MODE_VIEW = 0, MODE_SKETCH, MODE_SELECT };

public:
	Ui::MyMainWindowClass ui;
	int mode;
	GLWidget* glWidget;
	ControlWidget* controlWidget;
	RoadBoxList* largeRoadBoxList;
	QDockWidget *dockLargeRoadBoxList;
	RoadBoxList* smallRoadBoxList;
	QDockWidget *dockSmallRoadBoxList;

public:
	MyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyMainWindow();

private slots:
	void newRoad();
	void openRoad();
	void showControlWidget();
	void showLargeReferenceRoads();
	void showSmallReferenceRoads();
};

#endif // MYMAINWINDOW_H
