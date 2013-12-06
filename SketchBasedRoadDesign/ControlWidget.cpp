#include "ControlWidget.h"
#include "MyMainWindow.h"
#include "RoadGraph.h"
#include "RoadDBView.h"

ControlWidget::ControlWidget(MyMainWindow* parent) : QDockWidget("Control", (QWidget*)parent) {
	this->parent = parent;

	ui.setupUi(this);

	// setup the signal handler
	connect(ui.pushButtonSearch, SIGNAL(clicked()), this, SLOT(search()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
}

ControlWidget::~ControlWidget() {
}

void ControlWidget::search() {
	//RoadGraph* roads = parent->view->buildRoads();

	RoadDBView* view = new RoadDBView(parent);
	view->show();
}

void ControlWidget::accept() {
}
