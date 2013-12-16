#include "MyMainWindow.h"
#include <QFileDialog>
#include <qgraphicsview.h>
#include <qlayout.h>

MyMainWindow::MyMainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// set up the control widget
	controlWidget = new ControlWidget(this);
	controlWidget->hide();

	// set up the road list widget
	dockRoadBoxList = new QDockWidget(tr("Reference Roads"), this);
	roadBoxList = new RoadBoxList(this);
	dockRoadBoxList->setWidget(roadBoxList);
	dockRoadBoxList->hide();

	// register signal handlers
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newRoad()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openRoad()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(showControlWidget()));
	connect(ui.actionReferenceRoads, SIGNAL(triggered()), this, SLOT(showReferenceRoads()));

	// setup the view
	canvas = new RoadCanvas(this, 3000, 3000);
	setCentralWidget(canvas);
}

MyMainWindow::~MyMainWindow() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handler2

void MyMainWindow::newRoad() {
	canvas->newRoad();
}

void MyMainWindow::openRoad() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	canvas->openRoad(filename.toAscii().constData());
	QApplication::restoreOverrideCursor();
}

void MyMainWindow::showControlWidget() {
	// display the control widget
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

void MyMainWindow::showReferenceRoads() {
	// display the reference roads
	dockRoadBoxList->show();
	addDockWidget(Qt::RightDockWidgetArea, dockRoadBoxList);
}
