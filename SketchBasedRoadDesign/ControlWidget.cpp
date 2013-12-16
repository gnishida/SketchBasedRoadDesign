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
	if (factor <= 1) mainWin->canvas->setZoom(RoadCanvas::ZOOM_OUT);
	else mainWin->canvas->setZoom(1.0f);

	mainWin->roadBoxList->updateWidget();
}

void ControlWidget::search() {
	GraphUtil::planarify(mainWin->canvas->sketch);
	for (int i = 0; i < mainWin->roadBoxList->references.size(); i++) {
		mainWin->roadBoxList->references[i]->view->showSimilarity(mainWin->canvas->sketch);
	}
	
	mainWin->canvas->update();
}

/**
 * Clear the screen.
 */
void ControlWidget::clear() {
	mainWin->canvas->sketch->clear();
	mainWin->canvas->updateView();
}

void ControlWidget::save() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save road network ..."), QString(), tr("GSM Files (*.gsm)"));
	if (filename != QString::null && !filename.isEmpty()) {
		FILE* fp = fopen(filename.toUtf8().data(), "wb");
		RoadGraph* roads = mainWin->canvas->sketch;
		roads->save(fp);
		fclose(fp);

		delete roads;
	}
}
