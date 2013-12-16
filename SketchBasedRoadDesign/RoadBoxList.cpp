#include "RoadBoxList.h"
#include "MyMainWindow.h"

RoadBoxList::RoadBoxList(MyMainWindow* mainWin) : QWidget((QWidget*)mainWin) {
	this->mainWin = mainWin;
	//ui.setupUi(this);

	QVBoxLayout* layout = new QVBoxLayout();
	this->setLayout(layout);

	updateWidget();
}

RoadBoxList::~RoadBoxList() {
}

void RoadBoxList::updateWidget() {
	for (int i = 0; i < references.size(); i++) {
		this->layout()->removeWidget(references[i]);
	}
	references.clear();
	
	const char* largeFilename[3] = {"osm\\15x15\\beijing.gsm", "osm\\15x15\\canberra.gsm", "osm\\15x15\\london.gsm"};
	const char* smallFilename[3] = {"osm\\3x3\\beijing.gsm", "osm\\3x3\\canberra.gsm", "osm\\3x3\\london.gsm"};

	if  (mainWin->canvas->zoom == RoadCanvas::ZOOM_IN) {
		for (int i = 0; i < 3; i++) {
			RoadBox* box = new RoadBox(mainWin, smallFilename[i]);
			this->layout()->addWidget(box);
			references.push_back(box);
		}
	} else {
		for (int i = 0; i < 3; i++) {
			RoadBox* box = new RoadBox(mainWin, largeFilename[i]);
			this->layout()->addWidget(box);
			references.push_back(box);
		}
	}
}
