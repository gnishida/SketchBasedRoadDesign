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

	// ３つの道路網サンプルのためのビューを追加する
	for (int i = 0; i < 3; i++) {
		RoadDBView* v = new RoadDBView(sub_win);
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
	// DocWidgetの表示
	controlWidget->show();
	addDockWidget(Qt::RightDockWidgetArea, controlWidget);
}
