#pragma once

#include "ui_ControlWidget.h"
#include <qdockwidget.h>

class MyMainWindow;

class ControlWidget : public QDockWidget {
Q_OBJECT

protected:
	MyMainWindow* mainWin;
	Ui::ControlWidget ui;

public:
	ControlWidget(MyMainWindow* mainWin);
	~ControlWidget();
	void updateModeButtons();

public slots:
	void modeView(bool flag);
	void modeSketch(bool flag);
	void modeSelect(bool flag);
	void zoom(int factor);
	void search();
	void accept();
	void clear();
	void save();
};

