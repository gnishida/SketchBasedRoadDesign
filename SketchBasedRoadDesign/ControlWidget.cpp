#include "ControlWidget.h"
#include "MyMainWindow.h"
#include "RoadGraph.h"
#include "RoadDBView.h"
#include "GraphUtil.h"
#include <qfiledialog.h>
#include <limits>

ControlWidget::ControlWidget(MyMainWindow* parent) : QDockWidget("Control", (QWidget*)parent) {
	this->parent = parent;

	ui.setupUi(this);

	// setup the signal handler
	connect(ui.pushButtonSearch, SIGNAL(clicked()), this, SLOT(search()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.pushButtonClear, SIGNAL(clicked()), this, SLOT(clear()));
	connect(ui.pushButtonSave, SIGNAL(clicked()), this, SLOT(save()));
}

ControlWidget::~ControlWidget() {
}

void ControlWidget::search() {
	RoadGraph* roads = parent->view->sketchToRoads();

	for (int i = 0; i < parent->examples.size(); i++) {
		parent->examples[i]->showSimilarity(roads);
	}
}

void ControlWidget::accept() {
	RoadGraph* roads = parent->view->sketchToRoads();

	float max_similarity = 0;
	RoadDBView* max_view;

	for (int i = 0; i < parent->examples.size(); i++) {
		float similarity = parent->examples[i]->showSimilarity(roads);
		if (similarity < max_similarity) {
			max_similarity = similarity;
			max_view = parent->examples[i];
		}
	}

	parent->view->setReferene(max_view->roads);
}

void ControlWidget::clear() {
	parent->view->clear();
}

void ControlWidget::save() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save road network ..."), QString(), tr("GSM Files (*.gsm)"));
	if (filename != QString::null && !filename.isEmpty()) {
		FILE* fp = fopen(filename.toUtf8().data(), "wb");
		RoadGraph* roads = parent->view->sketchToRoads();
		roads->save(fp);
		fclose(fp);

		delete roads;
	}
}
