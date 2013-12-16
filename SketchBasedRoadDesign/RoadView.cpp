#include "RoadView.h"
//#include "Line.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include <qmap.h>
#include <QLineF>
#include <QGraphicsSimpleTextItem>

RoadView::RoadView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 3000, 3000);
	//this->setSceneRect(0, 0, 200, 200);
	//this->setMaximumSize(200, 200);
	this->setFixedSize(200, 200);

	this->setScene(scene);

	this->setBackgroundBrush(QColor(233, 229, 220));
	
	this->scale(0.04, 0.04);

	setRenderHint(QPainter::Antialiasing);

	roads = NULL;
}

RoadView::~RoadView() {
}

void RoadView::load(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (roads != NULL) {
		delete roads;
	}
	roads = new RoadGraph();
	roads->load(fp, 7);
	fclose(fp);

	QPen pen(QColor(0, 0, 255));

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		if (roads->graph[*ei]->type != 2) continue;

		for (int i = 0; i < roads->graph[*ei]->polyLine.size() - 1; i++) {
			QLineF line(roads->graph[*ei]->polyLine[i].x(), -roads->graph[*ei]->polyLine[i].y(), roads->graph[*ei]->polyLine[i+1].x(), -roads->graph[*ei]->polyLine[i+1].y());
			line.translate(1500, 1500);
			scene->addLine(line, pen);
		}		
	}

	// Draw a square for the central vertex
	//RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	//scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));

	scene->update();
}

float RoadView::showSimilarity(RoadGraph* roads2) {
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
	updateView(r1);

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
void RoadView::updateView(RoadGraph* roads) {
	scene->clear();

	QPen pen(QColor(0, 0, 255));

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		if (roads->graph[*ei]->type != 2) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		for (int i = 0; i < roads->graph[*ei]->polyLine.size() - 1; i++) {
			QLineF line(roads->graph[*ei]->polyLine[i].x(), -roads->graph[*ei]->polyLine[i].y(), roads->graph[*ei]->polyLine[i+1].x(), -roads->graph[*ei]->polyLine[i+1].y());
			line.translate(1500, 1500);
			QGraphicsLineItem* item = scene->addLine(line, pen);
			if (roads->graph[*ei]->fullyPaired) {
				item->setOpacity(1);
			} else {
				item->setOpacity(0.1);
			}
		}
	}

	// Draw the square for the central vertex
	//RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	//scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));
}
