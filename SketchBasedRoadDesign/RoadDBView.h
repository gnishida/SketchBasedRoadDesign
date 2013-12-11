#pragma once

#include "RoadGraph.h"
#include <qgraphicsview.h>

class RoadDBView : public QGraphicsView {
public:
	QGraphicsScene* scene;
	//QGraphicsSimpleTextItem* score;
	RoadGraph* roads;

public:
	RoadDBView(QWidget* parent = 0);
	~RoadDBView();

	void load(const char* filename);
	float showSimilarity(RoadGraph* roads);
	void updateView(RoadGraph* roads);
};

