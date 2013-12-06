#include "MyGraphicsView.h"
#include "Util.h"
#include "GraphUtil.h"
#include <qvector2d.h>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 800, 600);
	this->setScene(scene);

	setRenderHint( QPainter::Antialiasing );
}

MyGraphicsView::~MyGraphicsView() {
}

/**
 * スケッチを元に、道路網を生成する。
 */
RoadGraph* MyGraphicsView::buildRoads() {
	RoadGraph* roads = new RoadGraph();

	for (int i = 0; i < scene->items().size(); i++) {
		Line* line = (Line*)scene->items()[i];
		if (line->points.size() < 2) continue;

		RoadVertex* v1 = new RoadVertex(line->points[0]);
		RoadVertexDesc v1_desc = boost::add_vertex(roads->graph);
		roads->graph[v1_desc] = v1;

		RoadVertex* v2 = new RoadVertex(line->points[line->points.size() - 1]);
		RoadVertexDesc v2_desc = boost::add_vertex(roads->graph);
		roads->graph[v2_desc] = v2;

		RoadEdgeDesc e_desc = GraphUtil::addEdge(roads, v1_desc, v2_desc, 2, 2, false);
		roads->graph[e_desc]->polyLine = line->points;
	}

	GraphUtil::planarify(roads);
	GraphUtil::simplify(roads, 4.0f);
	GraphUtil::clean(roads);

	return roads;
}

void MyGraphicsView::mousePressEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		currentLine = new Line();
		scene->addItem(currentLine);

		QPointF pt = mapToScene(e->pos());

		currentLine->points.push_back(QVector2D(pt.x(), pt.y()));
	} else if (e->buttons() == Qt::RightButton) {
	}

	scene->update();
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent* e) {
	if (currentLine != NULL) {
		currentLine->simplify();

		//RoadGraph* roads = buildRoads();
	}

	scene->update();
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		QPointF pt = mapToScene(e->pos());
		currentLine->points.push_back(QVector2D(pt.x(), pt.y()));
	}

	scene->update();
}
