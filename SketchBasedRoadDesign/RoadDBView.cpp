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

	// 中心頂点を求め、大きい四角で表示
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));

	scene->update();
}

float RoadDBView::showSimilarity(RoadGraph* roads2) {
	// 各道路網のImportanceを計算する
	GraphUtil::computeImportanceOfEdges(roads, 1.0f, 1.0f, 1.0f);
	GraphUtil::computeImportanceOfEdges(roads2, 1.0f, 1.0f, 1.0f);

	// まず、それぞれの中心頂点を求める
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	RoadVertexDesc v2 = GraphUtil::getCentralVertex(roads2);

	// 木構造を作成
	BFSTree tree1(roads, v1);
	BFSTree tree2(roads2, v2);

	// マッチングを探す
	QMap<RoadVertexDesc, RoadVertexDesc> map1;
	QMap<RoadVertexDesc, RoadVertexDesc> map2;
	GraphUtil::findCorrespondence(roads, &tree1, roads2, &tree2, false, map1, map2);

	// マッチングに基づいて、道路網の表示を更新する
	updateView();

	// 類似度を計算する
	float similarity = GraphUtil::computeSimilarity(roads, map1, roads2, map2);
	QString str;
	str.setNum(similarity);
	//score->setText(str);
	QGraphicsSimpleTextItem* score = scene->addSimpleText(str, QFont("Times", 1000));
	score->setPen(QPen(Qt::blue));
	score->setPos(0, 0);

	update();

	return similarity;
}

/**
 * マッチングに基づいて、道路網の表示を更新する。
 * 対応相手があるエッジは赤色で、その他は、黒色で表示する。
 */
void RoadDBView::updateView() {
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

	// 中心頂点を求め、大きい四角で表示
	RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));
}
