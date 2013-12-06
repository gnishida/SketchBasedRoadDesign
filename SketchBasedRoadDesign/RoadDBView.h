#pragma once

#include <qgraphicsview.h>

class RoadDBView : public QGraphicsView {
private:
	QGraphicsScene* scene;

public:
	RoadDBView(QWidget* parent = 0);
	~RoadDBView();
};

