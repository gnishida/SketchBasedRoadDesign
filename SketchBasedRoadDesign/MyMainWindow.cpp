#include "MyMainWindow.h"
//#include "Canvas.h"
#include <qgraphicsview.h>

MyMainWindow::MyMainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	view = new MyGraphicsView(this);

	setCentralWidget(view);

	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(showControlWidget()));

	controlWidget = new ControlWidget(this);
	controlWidget->hide();
}

MyMainWindow::~MyMainWindow() {

}

void MyMainWindow::showControlWidget() {
	// DocWidgetの表示
	controlWidget->show();
	addDockWidget(Qt::RightDockWidgetArea, controlWidget);
}
