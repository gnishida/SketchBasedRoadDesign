#include "RoadCanvas.h"
#include "MyMainWindow.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include <qmap.h>
#include <QMouseEvent>
#include <QLineF>
#include <QGraphicsSimpleTextItem>
#include <qdebug.h>

float RoadCanvas::ZOOM_IN = 1.0f;
float RoadCanvas::ZOOM_OUT = 0.2f;

RoadCanvas::RoadCanvas(MyMainWindow *mainWin, float width, float height) : QGraphicsView((QWidget*)mainWin) {
	this->mainWin = mainWin;
	this->width = width;
	this->height = height;
	this->zoom = ZOOM_OUT;
	this->snapThreshold = 20.0f;

	// set up the GUI
	this->setSceneRect(0, 0, width, height);
	this->setBackgroundBrush(QColor(233, 229, 220));
	this->scale(zoom, zoom);
	this->setRenderHint(QPainter::Antialiasing);
	//this->setFixedSize(width * zoom, height * zoom);

	// set up the scene
	scene = new QGraphicsScene();
	this->setScene(scene);

	// set up the road objects and the sketch object
	roads = new RoadGraph();
	ref_roads = NULL;
	sketch = new Sketch();
}

RoadCanvas::~RoadCanvas() {
}

/**
 * Delete the existing roads and start a new road from scratch.
 */
void RoadCanvas::newRoad() {
	if (roads != NULL) delete roads;
	roads = new RoadGraph();

	updateView();
}

/**
 * Open a road file.
 */
void RoadCanvas::openRoad(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (roads != NULL) {
		delete roads;
	}
	roads = new RoadGraph();
	roads->load(fp, 7);
	fclose(fp);

	updateView();
}

float RoadCanvas::showSimilarity(RoadGraph* roads2) {
	RoadGraph* r1 = GraphUtil::copyRoads(roads);
	RoadGraph* r2 = GraphUtil::copyRoads(roads2);

	// Compute the importance of each edge
	//GraphUtil::computeImportanceOfEdges(r1, 1.0f, 1.0f, 1.0f);
	//GraphUtil::computeImportanceOfEdges(r2, 1.0f, 1.0f, 1.0f);

	// Find the central vertex
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(r1);
	RoadVertexDesc v2 = GraphUtil::getCentralVertex(r2);

	// Create a tree
	BFSTree tree1(r1, v1);
	BFSTree tree2(r2, v2);

	// Find the matching
	QMap<RoadVertexDesc, RoadVertexDesc> map1;
	QMap<RoadVertexDesc, RoadVertexDesc> map2;
	GraphUtil::findCorrespondence(r1, &tree1, r2, &tree2, false, 0.75f, map1, map2);

	// Update the view based on the matching
	updateView();

	// Compute the similarity
	float similarity = GraphUtil::computeSimilarity(r1, map1, r2, map2, 1.0f, 5.0f);
	QString str;
	str.setNum(similarity);
	//score->setText(str);
	QGraphicsSimpleTextItem* score = scene->addSimpleText(str, QFont("Times", 1000));
	score->setPen(QPen(Qt::blue));
	score->setPos(0, 0);

	// Delete the temporal roads
	delete r1;
	delete r2;

	update();

	return similarity;
}

/**
 * Update the view based on the road graph with matching infromation.
 * If the edge has a corresponding one, color it with red. Otherwise, color itt with black.
 */
void RoadCanvas::updateView() {
	scene->clear();

	drawFancyRoads(roads);
	drawSimpleRoads(sketch);
	if (ref_roads != NULL) {
		drawFancyRoads(ref_roads);
	}

	scene->update();
}

/**
 * Draw the fancy roads based on the specified road graph.
 */
void RoadCanvas::drawFancyRoads(RoadGraph* roads) {
	// define the road width base
	float widthBase = 10.0f;
	float border = 4.0f;
	if (zoom < 1.0f) {
		widthBase = 14.0f;
		border = 7.0f;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		//if (roads->graph[*ei]->type != 2) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		// define the color
		QColor color, borderColor;
		switch (roads->graph[*ei]->type) {
		case 3:	// high way
			color = QColor(255, 225, 104);
			borderColor = QColor(229, 153, 21);
			break;
		case 2: // avenue
		case 1: // street
			color = QColor(255, 255, 255);
			borderColor = QColor(217, 209, 201);
			break;
		}
		
		if (zoom < 0.1f && roads->graph[*ei]->type == 1) {
			drawEdge(roads, *ei, widthBase * 0.5f, 0.0f, 0.0f, borderColor);
		} else {
			drawEdge(roads, *ei, widthBase + border, 0.0f, 0.0f, borderColor);
			drawEdge(roads, *ei, widthBase, 1.0f, 0.5f, color);
		}
	}

	// Draw the square for the central vertex
	//RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	//scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));
}

/**
 * Draw the simple roads based on the specified road graph.
 */
void RoadCanvas::drawSimpleRoads(RoadGraph* roads) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		//if (roads->graph[*ei]->type != 2) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		drawEdge(roads, *ei, 1.0f, 0.0f, 0.0f, QColor(0, 0, 255));
	}

	// Draw the square for the central vertex
	//RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	//scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));
}

void RoadCanvas::drawEdge(RoadGraph* roads, RoadEdgeDesc edge, float roadWidthBase, float highwayZ, float avenueZ, const QColor& color) {
	QPen pen(color);
	QBrush brush(color);

	// define the width and Z of the road segment
	float width;
	float z;
	switch (roads->graph[edge]->type) {
	case 3:	// high way
		width = roadWidthBase * 2.0f;
		z = highwayZ;
		break;
	case 2: // avenue
		width = roadWidthBase * 1.5f;
		z = avenueZ;
		break;
	case 1: // local street
		width = roadWidthBase * 1.0f;
		z = avenueZ;
		break;
	}

	pen.setWidthF(width);
	pen.setCapStyle(Qt::FlatCap);
	
	QVector2D pt0 = modelToScene(roads->graph[edge]->polyLine[0]);
	QPainterPath path(QPointF(pt0.x(), pt0.y()));
	for (int i = 0; i < roads->graph[edge]->polyLine.size(); i++) {
		QVector2D pt = modelToScene(roads->graph[edge]->polyLine[i]);
		path.lineTo(pt.x(), pt.y());
	}
	QGraphicsPathItem* item = scene->addPath(path, pen);
	item->setZValue(z);

	/*
	for (int i = 0; i < roads->graph[edge]->polyLine.size() - 1; i++) {
		QVector2D pt1 = modelToScene(roads->graph[edge]->polyLine[i]);
		QVector2D pt2 = modelToScene(roads->graph[edge]->polyLine[i + 1]);
		QVector2D vec = pt2 - pt1;
		vec = QVector2D(-vec.y(), vec.x());
		vec.normalize();
		
		QVector2D p0 = pt1 + vec * width * 0.5f;
		QVector2D p1 = pt1 - vec * width * 0.5f;
		QVector2D p2 = pt2 - vec * width * 0.5f;
		QVector2D p3 = pt2 + vec * width * 0.5f;

		QVector<QPointF> points;
		points.push_back(QPoint(p0.x(), p0.y()));
		points.push_back(QPoint(p1.x(), p1.y()));
		points.push_back(QPoint(p2.x(), p2.y()));
		points.push_back(QPoint(p3.x(), p3.y()));
		
		QGraphicsPolygonItem* item = scene->addPolygon(QPolygonF(points), pen, brush);
		item->setZValue(z);
	}
	*/
}

void RoadCanvas::setZoom(float factor) {
	if (zoom == factor) return;

	this->scale(factor / zoom, factor / zoom);
	zoom = factor;

	updateView();
}

/**
 * Convert the road graph coordinate to the scene coordinate.
 * [(-width/2.0f, -height/2.0f), (width/2.0f, height/2.0f)] => [(0, 0), (width, height)]
 */
QVector2D RoadCanvas::modelToScene(const QVector2D& pt) {
	QVector2D ret;
	ret.setX(pt.x() + width / 2.0f);
	ret.setY(height / 2.0f - pt.y());
	return ret;
}

QVector2D RoadCanvas::sceneToModel(const QPointF& pt) {
	QVector2D ret;
	ret.setX(pt.x() - width / 2.0f);
	ret.setY(height / 2.0f - pt.y());
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handlers

void RoadCanvas::mousePressEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		if (mainWin->mode == MyMainWindow::MODE_SKETCH) {
			QVector2D pos = sceneToModel(mapToScene(e->pos()));

			RoadVertexDesc v1_desc;
			if (!GraphUtil::getVertex(sketch, pos, snapThreshold / zoom, v1_desc)) {
				// If there is a vertex close to the point, don't add new vertex. Otherwise, add a new vertex.
				RoadVertex* v1 = new RoadVertex(pos);
				v1_desc = boost::add_vertex(sketch->graph);
				sketch->graph[v1_desc] = v1;
			}

			// add the 2nd vertex of a line
			RoadVertex* v2 = new RoadVertex(pos);
			RoadVertexDesc v2_desc = boost::add_vertex(sketch->graph);
			sketch->graph[v2_desc] = v2;

			sketch->curVertex = v2_desc;

			// add an edge
			sketch->curEdge = GraphUtil::addEdge(sketch, v1_desc, v2_desc, 1, 1, false);
		}
	} else if (e->buttons() == Qt::RightButton) {
	}

	updateView();
}

void RoadCanvas::mouseReleaseEvent(QMouseEvent* e) {
	if (mainWin->mode == MyMainWindow::MODE_SKETCH) {
		QVector2D pos = sceneToModel(mapToScene(e->pos()));
		RoadVertexDesc v2_desc;
		
		if (GraphUtil::getVertex(sketch, pos, snapThreshold / zoom, sketch->curVertex, v2_desc)) {
			// If there is a vertex close to the point, snap the point to the nearest vertex
			GraphUtil::snapVertex(sketch, sketch->curVertex, v2_desc);

			// If the length of the line is 0, remove it.
			if (sketch->graph[sketch->curEdge]->getLength() == 0.0f) {
				RoadVertexDesc src = boost::source(sketch->curEdge, sketch->graph);
				RoadVertexDesc tgt = boost::target(sketch->curEdge, sketch->graph);

				sketch->graph[src]->valid = false;
				sketch->graph[tgt]->valid = false;
				sketch->graph[sketch->curEdge]->valid = false;
			}
		}
	}

	updateView();
}

void RoadCanvas::mouseMoveEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		if (mainWin->mode == MyMainWindow::MODE_SKETCH) {
			QVector2D pos = sceneToModel(mapToScene(e->pos()));
			GraphUtil::moveVertex(sketch, sketch->curVertex, pos);
		}
	}

	updateView();
}
