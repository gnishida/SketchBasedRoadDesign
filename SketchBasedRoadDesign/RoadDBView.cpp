#include "RoadDBView.h"
#include "Line.h"
#include "GraphUtil.h"

RoadDBView::RoadDBView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(-5000, -5000, 5000, 5000);

	this->setScene(scene);

	this->setBackgroundBrush(Qt::red);
	this->scale(0.012, 0.012);

	setRenderHint(QPainter::Antialiasing);

	roads = NULL;
}

RoadDBView::~RoadDBView() {
}

void RoadDBView::load(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (roads != NULL) {
		delete roads;
	}
	roads = new RoadGraph();
	roads->load(fp, 2);
	fclose(fp);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		Line* line = new Line();
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			line->points.push_back(roads->graph[*ei]->polyLine[i]);
		}

		scene->addItem(line);
	}

	scene->update();
}

void RoadDBView::showDissimilarity(RoadGraph* roads) {
	//GraphUtil::computeDissimilarity2();
}
