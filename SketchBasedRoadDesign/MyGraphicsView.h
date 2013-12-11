#pragma once

#include "Line.h"
#include "RoadGraph.h"
#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <QMouseEvent>

class MyGraphicsView : public QGraphicsView {
Q_OBJECT

private:
	QGraphicsScene* scene;
	Line* currentLine;
	RoadGraph* roads;

public:
	MyGraphicsView(QWidget* parent = 0);
	~MyGraphicsView();

public:
	RoadGraph* sketchToRoads();
	void roadsToSketch();
	void setReferene(RoadGraph* ref_roads);
	void clear();

signals:

public slots:
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);

private:
};

