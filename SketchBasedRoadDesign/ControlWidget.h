#pragma once

#include "ui_ControlWidget.h"
#include <qdockwidget.h>

class MyMainWindow;

class ControlWidget : public QDockWidget {
Q_OBJECT

protected:
	MyMainWindow* parent;
	Ui::ControlWidget ui;

public:
	ControlWidget(MyMainWindow* parent);
	~ControlWidget();

public slots:
	void search();
	void accept();
	void clear();
	void save();
};

