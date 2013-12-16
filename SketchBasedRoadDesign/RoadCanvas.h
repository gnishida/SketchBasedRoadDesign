#pragma once

#include "RoadGraph.h"
#include "Sketch.h"
#include <qgraphicsview.h>

class MyMainWindow;

class RoadCanvas : public QGraphicsView {
public:
	static float ZOOM_IN;
	static float ZOOM_OUT;

public:
	MyMainWindow* mainWin;
	float width;
	float height;
	float zoom;
	QGraphicsScene* scene;
	RoadGraph* roads;
	RoadGraph* ref_roads;
	Sketch* sketch;
	float snapThreshold;

public:
	RoadCanvas(MyMainWindow* mainWin, float width, float height);
	~RoadCanvas();

	void newRoad();
	void openRoad(const char* filename);
	float showSimilarity(RoadGraph* roads);
	void updateView();
	void drawFancyRoads(RoadGraph* roads);
	void drawSimpleRoads(RoadGraph* roads);
	void drawEdge(RoadGraph* roads, RoadEdgeDesc edge, float roadWidthBase, float highwayZ, float avenueZ, const QColor& color);

	void setZoom(float factor);
	QVector2D modelToScene(const QVector2D& pt);
	QVector2D sceneToModel(const QPointF& pt);

signals:

public slots:
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
};

