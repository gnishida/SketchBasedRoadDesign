#include "MyMainWindow.h"
#include <QFileDialog>
#include <qgraphicsview.h>
#include <qlayout.h>

MyMainWindow::MyMainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// set up the control widget
	controlWidget = new ControlWidget(this);
	controlWidget->hide();

	// set up the large road list widget
	std::vector<QString> largeRoads;
	largeRoads.push_back("osm\\3x3\\beijing.gsm");
	largeRoads.push_back("osm\\3x3\\canberra.gsm");
	largeRoads.push_back("osm\\3x3\\london.gsm");
	/*
	largeRoads.push_back("osm\\15x15\\beijing.gsm");
	largeRoads.push_back("osm\\15x15\\canberra.gsm");
	largeRoads.push_back("osm\\15x15\\london.gsm");
	*/
	dockLargeRoadBoxList = new QDockWidget(tr("Reference Roads"), this);
	largeRoadBoxList = new RoadBoxList(this, largeRoads, 3000.0f);
	dockLargeRoadBoxList->setWidget(largeRoadBoxList);
	dockLargeRoadBoxList->hide();

	// set up the small road list widget
	std::vector<QString> smallRoads;
	smallRoads.push_back("osm\\1x1\\beijing.gsm");
	smallRoads.push_back("osm\\1x1\\canberra.gsm");
	smallRoads.push_back("osm\\1x1\\london.gsm");
	/*
	smallRoads.push_back("osm\\3x3\\beijing.gsm");
	smallRoads.push_back("osm\\3x3\\canberra.gsm");
	smallRoads.push_back("osm\\3x3\\london.gsm");
	*/
	dockSmallRoadBoxList = new QDockWidget(tr("Reference Roads"), this);
	smallRoadBoxList = new RoadBoxList(this, smallRoads, 1000.0f);
	dockSmallRoadBoxList->setWidget(smallRoadBoxList);
	dockSmallRoadBoxList->hide();

	// register signal handlers
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newRoad()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openRoad()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(showControlWidget()));
	connect(ui.actionLargeReferenceRoads, SIGNAL(triggered()), this, SLOT(showLargeReferenceRoads()));
	connect(ui.actionSmallReferenceRoads, SIGNAL(triggered()), this, SLOT(showSmallReferenceRoads()));

	// setup the GL widget
	glWidget = new GLWidget(this);
	setCentralWidget(glWidget);

	mode = MODE_VIEW;
}

MyMainWindow::~MyMainWindow() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handler2

void MyMainWindow::newRoad() {
	glWidget->newRoad();
}

void MyMainWindow::openRoad() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	glWidget->openRoad(filename.toAscii().constData());
	QApplication::restoreOverrideCursor();
}

/**
 * Display the control widget.
 */
void MyMainWindow::showControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

/**
 * Display the large reference roads.
 */
void MyMainWindow::showLargeReferenceRoads() {
	dockSmallRoadBoxList->hide();

	dockLargeRoadBoxList->show();
	addDockWidget(Qt::RightDockWidgetArea, dockLargeRoadBoxList);
}

/**
 * Display the small reference roads.
 */
void MyMainWindow::showSmallReferenceRoads() {
	dockLargeRoadBoxList->hide();

	dockSmallRoadBoxList->show();
	addDockWidget(Qt::RightDockWidgetArea, dockSmallRoadBoxList);
}
