#include "RoadDBView.h"
#include "Line.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include <qmap.h>

RoadDBView::RoadDBView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 10000, 10000);
	//this->setSceneRect(0, 0, 200, 200);
	//this->setMaximumSize(200, 200);
	this->setFixedSize(220, 220);

	this->setScene(scene);

	this->setBackgroundBrush(Qt::red);
	this->scale(0.02, 0.02);

	setRenderHint(QPainter::Antialiasing);

	roads = NULL;
	/*
	score = scene->addSimpleText("test", QFont("Times", 1000));
	score->setPen(QPen(Qt::blue));
	score->setPos(0, 0);
	*/
}

RoadDBView::~RoadDBView() {
}

void RoadDBView::load(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (roads != NULL) {
		delete roads;
	}
	roads = new RoadGraph();
	roads->load(fp, 2);
	fclose(fp);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		Line* line = new Line();
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			line->points.push_back(roads->graph[*ei]->polyLine[i] + QVector2D(5000, 5000));
		}

		scene->addItem(line);
	}

	// Draw a square for the central vertex
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));

	scene->update();
}

float RoadDBView::showSimilarity(RoadGraph* roads2) {
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
void RoadDBView::updateView(RoadGraph* roads) {
	scene->clear();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		Line* line = new Line();
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			line->points.push_back(roads->graph[*ei]->polyLine[i] + QVector2D(5000, 5000));
		}

		if (roads->graph[*ei]->fullyPaired) {
			line->setOpacity(1);
		} else {
			line->setOpacity(0.1);
		}

		scene->addItem(line);
	}

	// Draw the square for the central vertex
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));
}
