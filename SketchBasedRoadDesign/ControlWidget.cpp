#include "ControlWidget.h"
#include "MyMainWindow.h"
#include "RoadGraph.h"
#include "GraphUtil.h"
#include <qfiledialog.h>
#include <limits>

ControlWidget::ControlWidget(MyMainWindow* mainWin) : QDockWidget("Control", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	ui.setupUi(this);

	// setup the GUI
	ui.pushButtonModeView->setChecked(true);
	ui.pushButtonModeView->setCheckable(true);
	ui.pushButtonModeSketch->setCheckable(true);
	ui.pushButtonModeSelect->setCheckable(true);
	ui.horizontalSliderZoom->setRange(1, 2);
	ui.horizontalSliderZoom->setValue(1);
	ui.horizontalSliderZoom->setSingleStep(1);

	// setup the signal handler
	connect(ui.pushButtonModeView, SIGNAL(clicked(bool)), this, SLOT(modeView(bool)));
	connect(ui.pushButtonModeSketch, SIGNAL(clicked(bool)), this, SLOT(modeSketch(bool)));
	connect(ui.pushButtonModeSelect, SIGNAL(clicked(bool)), this, SLOT(modeSelect(bool)));
	connect(ui.horizontalSliderZoom, SIGNAL(valueChanged(int)), this, SLOT(zoom(int)));
	connect(ui.pushButtonSearch, SIGNAL(clicked()), this, SLOT(search()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui.pushButtonClear, SIGNAL(clicked()), this, SLOT(clear()));
	connect(ui.pushButtonSave, SIGNAL(clicked()), this, SLOT(save()));
}

ControlWidget::~ControlWidget() {
}

/**
 * Update the up/down appearance of buttons according to the current mode.
 */
void ControlWidget::updateModeButtons() {
	ui.pushButtonModeView->setChecked(false);
	ui.pushButtonModeSketch->setChecked(false);
	ui.pushButtonModeSelect->setChecked(false);

	switch (mainWin->mode) {
	case MyMainWindow::MODE_VIEW:
		ui.pushButtonModeView->setChecked(true);
		break;
	case MyMainWindow::MODE_SKETCH:
		ui.pushButtonModeSketch->setChecked(true);
		break;
	case MyMainWindow::MODE_SELECT:
		ui.pushButtonModeSelect->setChecked(true);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Event Handlers

/**
 * Change the mode to "View" mode.
 */
void ControlWidget::modeView(bool flag) {
	mainWin->mode = MyMainWindow::MODE_VIEW;
	updateModeButtons();
}

/**
 * Change the mode to "Sketch" mode.
 */
void ControlWidget::modeSketch(bool flag) {
	mainWin->mode = MyMainWindow::MODE_SKETCH;
	updateModeButtons();
}

/**
 * Change the mode to "Select" mode.
 */
void ControlWidget::modeSelect(bool flag) {
	mainWin->mode = MyMainWindow::MODE_SELECT;
	updateModeButtons();
}

void ControlWidget::zoom(int factor) {
	if (factor <= 1) mainWin->glWidget->camera->dz = GLWidget::MAX_Z;
	else mainWin->glWidget->camera->dz = GLWidget::MIN_Z;

	mainWin->glWidget->updateGL();
}

void ControlWidget::search() {
	GraphUtil::planarify(mainWin->glWidget->sketch);

	if (mainWin->glWidget->camera->dz < GLWidget::MAX_Z) {
		for (int i = 0; i < mainWin->smallRoadBoxList->references.size(); i++) {
			qDebug() << i;
			mainWin->smallRoadBoxList->references[i]->view->showSimilarity(mainWin->glWidget->sketch, 3000, true);
		}
	} else {
		for (int i = 0; i < mainWin->largeRoadBoxList->references.size(); i++) {
			qDebug() << i;
			mainWin->largeRoadBoxList->references[i]->view->showSimilarity(mainWin->glWidget->sketch, 3000, false);
		}
	}
	
	mainWin->glWidget->updateGL();
}

/**
 * Finalize the reference roads as the actual roads.
 */
void ControlWidget::accept() {
	if (mainWin->glWidget->ref_roads == NULL) return;

	GraphUtil::mergeRoads(mainWin->glWidget->roads, mainWin->glWidget->ref_roads);
	delete mainWin->glWidget->ref_roads;
	mainWin->glWidget->ref_roads = NULL;

	mainWin->glWidget->updateGL();
}

/**
 * Clear the screen.
 */
void ControlWidget::clear() {
	mainWin->glWidget->sketch->clear();
	mainWin->glWidget->updateGL();
}

void ControlWidget::save() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save road network ..."), QString(), tr("GSM Files (*.gsm)"));
	if (filename != QString::null && !filename.isEmpty()) {
		FILE* fp = fopen(filename.toUtf8().data(), "wb");
		RoadGraph* roads = mainWin->glWidget->sketch;
		roads->save(fp);
		fclose(fp);

		delete roads;
	}
}
