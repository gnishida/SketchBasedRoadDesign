#pragma once

#include "RoadGraph.h"
#include <qgraphicsview.h>

class RoadDBView : public QGraphicsView {
private:
	QGraphicsScene* scene;
	RoadGraph* roads;

public:
	RoadDBView(QWidget* parent = 0);
	~RoadDBView();

	void load(const char* filename);
	void showDissimilarity(RoadGraph* roads);
};

