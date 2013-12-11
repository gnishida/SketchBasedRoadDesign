#include "MyGraphicsView.h"
#include "Util.h"
#include "GraphUtil.h"
#include <qvector2d.h>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 800, 600);
	this->setScene(scene);

	setRenderHint(QPainter::Antialiasing);

	roads = NULL;
}

MyGraphicsView::~MyGraphicsView() {
}

/**
 * スケッチを元に、道路網を生成する。
 */
RoadGraph* MyGraphicsView::sketchToRoads() {
	if (roads != NULL) delete roads;

	roads = new RoadGraph();

	for (int i = 0; i < scene->items().size(); i++) {
		Line* line;
		try {
			line = dynamic_cast<Line*>(scene->items()[i]);
		} catch (const char* ex) {
		}

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
	GraphUtil::clean(roads);
	GraphUtil::simplify(roads, 20.0f);
	GraphUtil::clean(roads);

	// 作成された道路網に基づいて、スケッチを更新する
	roadsToSketch();

	update();

	return roads;
}

/**
 * 道路網データから、スケッチデータを生成する。
 */
void MyGraphicsView::roadsToSketch() {
	scene->clear();
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		Line* line = new Line();
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			line->points.push_back(roads->graph[*ei]->polyLine[i]);
		}

		scene->addItem(line);
	}

	// 中心頂点を求め、大きい四角で表示
	/*
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	scene->addRect(roads->graph[v1]->pt.x() - 3, roads->graph[v1]->pt.y() - 3, 6, 6, QPen(Qt::blue));
	*/
}

/**
 * ＤＢから参照となる道路をセットする。
 */
void MyGraphicsView::setReferene(RoadGraph* ref_roads) {
	scene->clear();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = edges(ref_roads->graph); ei != eend; ++ei) {
		if (!ref_roads->graph[*ei]->valid) continue;

		Line* line = new Line();
		for (int i = 0; i < ref_roads->graph[*ei]->polyLine.size(); i++) {
			line->points.push_back(ref_roads->graph[*ei]->polyLine[i]);
		}

		scene->addItem(line);
	}

	this->scale(0.1, 0.1);

	update();
}

/**
 * 画面をクリアする。
 */
void MyGraphicsView::clear() {
	scene->clear();

	if (roads != NULL) {
		delete roads;
		roads = NULL;
	}

	update();
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
