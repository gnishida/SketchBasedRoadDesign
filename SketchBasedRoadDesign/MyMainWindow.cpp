#include "MyMainWindow.h"
#include <qgraphicsview.h>
#include <qlayout.h>

MyMainWindow::MyMainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	view = new MyGraphicsView(this);

	QHBoxLayout* layout = new QHBoxLayout(this);
	QWidget* win = new QWidget();
	win->setLayout(layout);
	setCentralWidget(win);

	QVBoxLayout* db_layout = new QVBoxLayout(this);
	QWidget* sub_win = new QWidget();
	sub_win->setLayout(db_layout);

	// Add three views for the example road graphs
	const char* filename[3] = {"osm\\example1.gsm", "osm\\grid.gsm", "osm\\radial.gsm"};
	for (int i = 0; i < 3; i++) {
		RoadDBView* v = new RoadDBView(sub_win);
		v->load(filename[i]);
		examples.push_back(v);
		db_layout->addWidget(examples[i]);
	}
	
	layout->addWidget(view);
	layout->addWidget(sub_win);

	// control widget
	controlWidget = new ControlWidget(this);
	controlWidget->hide();

	// register signal handlers
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(showControlWidget()));
}

MyMainWindow::~MyMainWindow() {

}

void MyMainWindow::showControlWidget() {
	// display the control widget
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}
