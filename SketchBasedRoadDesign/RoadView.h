#pragma once

#include "RoadGraph.h"
#include <qgraphicsview.h>

class RoadView : public QGraphicsView {
public:
	QGraphicsScene* scene;
	RoadGraph* roads;

public:
	RoadView(QWidget* parent = 0);
	~RoadView();

	void load(const char* filename);
	float showSimilarity(RoadGraph* roads);
	void updateView(RoadGraph* roads);
};

