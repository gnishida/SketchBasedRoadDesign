#include "MyMainWindow.h"
//#include "Canvas.h"
#include <qgraphicsview.h>

MyMainWindow::MyMainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	/*
	canvas = new Canvas(this);
	
	view = new QGraphicsView(canvas);
	view->setRenderHint(QPainter::Antialiasing);
	view->setBackgroundBrush(Qt::FDiagPattern);

	view->setCacheMode(QGraphicsView::CacheBackground);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setDragMode(QGraphicsView::ScrollHandDrag);
	*/

	view = new MyGraphicsView(this);

	setCentralWidget(view);
}

MyMainWindow::~MyMainWindow() {

}
