#include "RoadDBView.h"
#include "Line.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include <qmap.h>

RoadDBView::RoadDBView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(-5000, -5000, 5000, 5000);

	this->setScene(scene);

	this->setBackgroundBrush(Qt::red);
	this->scale(0.012, 0.012);

	setRenderHint(QPainter::Antialiasing);

	roads = NULL;
	score = scene->addSimpleText("test", QFont("Times", 1400));
	score->setPen(QPen(Qt::blue));
	score->setPos(-5000, -5000);
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
			line->points.push_back(roads->graph[*ei]->polyLine[i]);
		}

		scene->addItem(line);
	}

	scene->update();
}

void RoadDBView::showDissimilarity(RoadGraph* roads2) {
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

	// 類似度を計算する
	float similarity = GraphUtil::computeSimilarity(roads, map1, roads2, map2);
	QString str;
	str.setNum(similarity);
	score->setText(str);

	update();
}
