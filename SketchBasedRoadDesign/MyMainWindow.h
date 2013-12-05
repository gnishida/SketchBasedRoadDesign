#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MyMainWindow.h"
//#include "Canvas.h"'
#include "MyGraphicsView.h"
#include <qscrollarea.h>

class MyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	QScrollArea* scrollArea;
	//Canvas* canvas;
	//QGraphicsView* view;
	MyGraphicsView* view;

public:
	MyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MyMainWindow();

private:
	Ui::MyMainWindowClass ui;
};

#endif // MYMAINWINDOW_H
