#include "ControlWidget.h"
#include "MyMainWindow.h"
#include "RoadGraph.h"
#include "RoadDBView.h"
#include <qfiledialog.h>
#include <limits>

ControlWidget::ControlWidget(MyMainWindow* parent) : QDockWidget("Control", (QWidget*)parent) {
	this->parent = parent;

	ui.setupUi(this);

	// setup the signal handler
	connect(ui.pushButtonSearch, SIGNAL(clicked()), this, SLOT(search()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.pushButtonSave, SIGNAL(clicked()), this, SLOT(save()));
}

ControlWidget::~ControlWidget() {
}

void ControlWidget::search() {
	RoadGraph* roads = parent->view->sketchToRoads();

	float min_similarity = std::numeric_limits<float>::max();
	RoadDBView* min_view;

	for (int i = 0; i < parent->examples.size(); i++) {
		float similarity = parent->examples[i]->showSimilarity(roads);
		if (similarity < min_similarity) {
			min_similarity = similarity;
			min_view = parent->examples[i];
		}
	}

	parent->view->setReferene(min_view->roads);

	delete roads;
}

void ControlWidget::accept() {
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
