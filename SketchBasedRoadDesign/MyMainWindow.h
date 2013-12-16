#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MyMainWindow.h"
#include "RoadCanvas.h"
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
	RoadCanvas* canvas;
	ControlWidget* controlWidget;
	RoadBoxList* roadBoxList;
	QDockWidget *dockRoadBoxList;

public:
	MyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyMainWindow();

private slots:
	void newRoad();
	void openRoad();
	void showControlWidget();
	void showReferenceRoads();
};

#endif // MYMAINWINDOW_H
