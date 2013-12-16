#include "RoadGraph.h"
#include "GraphUtil.h"
#include <qset.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#define SQR(x)		((x) * (x))

using namespace std;

RoadGraph::RoadGraph() {
}

RoadGraph::~RoadGraph() {
	clear();
}

void RoadGraph::generateMesh(float widthBase, float highwayHeight, float avenueHeight, float curbRatio, bool drawLocalStreets, float opacity) {
	renderables.clear();

	renderables.push_back(Renderable(GL_TRIANGLES));

	// road edge
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];

		QColor color, bColor;
		float height;
		switch (edge->type) {
		case 3:	// high way
			color = QColor(255, 225, 104, 255 * opacity);
			bColor = QColor(229, 153, 21, 255 * opacity);
			height = highwayHeight;
			break;
		case 2: // avenue
		case 1: // street
			color = QColor(255, 255, 255, 255 * opacity);
			bColor = QColor(217, 209, 201, 255 * opacity);
			height = avenueHeight;
			break;
		}

		// draw the border of the road segment
		if (!drawLocalStreets && edge->type == 1) {
			// If this is the local street and it should be drawn in gray color, it should be a little narrow line.
			addMeshFromEdge(&renderables[0], edge, widthBase * 0.6f, bColor, 0.0f);
		} else {
			addMeshFromEdge(&renderables[0], edge, widthBase * (1.0f + curbRatio), bColor, 0.0f);
			addMeshFromEdge(&renderables[0], edge, widthBase, color, height);
		}
	}

	renderables.push_back(Renderable(GL_POINTS, 20.0f));

	modified = false;
}

/**
 * Add a mesh for the specified edge.
 */
void RoadGraph::addMeshFromEdge(Renderable* renderable, RoadEdge* edge, float widthBase, QColor color, float height) {
	Vertex v;

	// define the width of the road segment
	float width;
	switch (edge->type) {
	case 3: // high way
		width = widthBase * 2.0f;
		break;
	case 2: // avenue
		width = widthBase * 1.5f;
		break;
	case 1: // local street
		width = widthBase * 1.0f;
		break;
	}

	int num = edge->polyLine.size();

	// draw the edge
	for (int i = 0; i < num - 1; ++i) {
		QVector2D pt1 = edge->polyLine[i];
		QVector2D pt2 = edge->polyLine[i + 1];
		QVector2D vec = pt2 - pt1;
		vec = QVector2D(-vec.y(), vec.x());
		vec.normalize();

		QVector2D p0 = pt1 + vec * width * 0.5f;
		QVector2D p1 = pt1 - vec * width * 0.5f;
		QVector2D p2 = pt2 - vec * width * 0.5f;
		QVector2D p3 = pt2 + vec * width * 0.5f;

		v.color[0] = color.redF();
		v.color[1] = color.greenF();
		v.color[2] = color.blueF();
		v.color[3] = color.alphaF();
		v.location[2] = height;

		v.location[0] = p0.x();
		v.location[1] = p0.y();
		renderables[0].vertices.push_back(v);

		v.location[0] = p1.x();
		v.location[1] = p1.y();
		renderables[0].vertices.push_back(v);

		v.location[0] = p2.x();
		v.location[1] = p2.y();
		renderables[0].vertices.push_back(v);

		v.location[0] = p0.x();
		v.location[1] = p0.y();
		renderables[0].vertices.push_back(v);

		v.location[0] = p2.x();
		v.location[1] = p2.y();
		renderables[0].vertices.push_back(v);

		v.location[0] = p3.x();
		v.location[1] = p3.y();
		renderables[0].vertices.push_back(v);
	}
}

/**
 * Clear the road graph.
 */
void RoadGraph::clear() {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];
		delete v;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];
		delete edge;
	}

	graph.clear();
}

/**
 * Load the road graph from a file.
 */
void RoadGraph::load(FILE* fp, int roadType) {
	clear();

	QMap<uint, RoadVertexDesc> idToDesc;

	// Read the number of vertices
	unsigned int nVertices;
	fread(&nVertices, sizeof(unsigned int), 1, fp);

	// Read each vertex's information: desc, x, and y.
	for (int i = 0; i < nVertices; i++) {
		RoadVertexDesc id;
		float x, y;
		fread(&id, sizeof(RoadVertexDesc), 1, fp);
		fread(&x, sizeof(float), 1, fp);
		fread(&y, sizeof(float), 1, fp);

		RoadVertex* vertex = new RoadVertex(QVector2D(x, y));

		RoadVertexDesc desc = boost::add_vertex(graph);
		graph[desc] = vertex;

		idToDesc[id] = desc;
	}

	// Read the number of edges
	unsigned int nEdges;
	fread(&nEdges, sizeof(unsigned int), 1, fp);

	// Read each edge's information: the descs of two vertices, road type, the number of lanes, the number of points along the polyline, and the coordinate of each point along the polyline.
	for (int i = 0; i < nEdges; i++) {
		RoadEdge* edge = new RoadEdge(1, 1, false);

		RoadVertexDesc id1, id2;
		fread(&id1, sizeof(RoadVertexDesc), 1, fp);
		fread(&id2, sizeof(RoadVertexDesc), 1, fp);

		RoadVertexDesc src = idToDesc[id1];
		RoadVertexDesc tgt = idToDesc[id2];

		fread(&edge->type, sizeof(unsigned int), 1, fp);
		fread(&edge->lanes, sizeof(unsigned int), 1, fp);
		fread(&edge->oneWay, sizeof(unsigned int), 1, fp);

		unsigned int nPoints;
		fread(&nPoints, sizeof(unsigned int), 1, fp);

		for (int j = 0; j < nPoints; j++) {
			float x, y;
			fread(&x, sizeof(float), 1, fp);
			fread(&y, sizeof(float), 1, fp);

			edge->addPoint(QVector2D(x, y));
		}

		// 指定されたタイプの道路エッジのみを読み込む
		if (((int)powf(2, (edge->type - 1)) & roadType)) {
			std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(src, tgt, graph);
			graph[edge_pair.first] = edge;
		} else {
			delete edge;
		}
	}
}

/**
 * Save the road graph to a file.
 */
void RoadGraph::save(FILE* fp) {
	int nVertices = GraphUtil::getNumVertices(this);
	fwrite(&nVertices, sizeof(int), 1, fp);

	// 各頂点につき、ID、X座標、Y座標を出力する
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		RoadVertex* v = graph[*vi];
		if (!v->valid) continue;
	
		RoadVertexDesc desc = *vi;
		float x = v->getPt().x();
		float y = v->getPt().y();
		fwrite(&desc, sizeof(RoadVertexDesc), 1, fp);
		fwrite(&x, sizeof(float), 1, fp);
		fwrite(&y, sizeof(float), 1, fp);
	}

	int nEdges = GraphUtil::getNumEdges(this);
	fwrite(&nEdges, sizeof(int), 1, fp);

	// 各エッジにつき、２つの頂点の各ID、道路タイプ、レーン数、一方通行か、ポリラインを構成するポイント数、各ポイントのX座標とY座標を出力する
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];
		if (!edge->valid) continue;

		RoadVertexDesc src = boost::source(*ei, graph);
		RoadVertexDesc tgt = boost::target(*ei, graph);

		fwrite(&src, sizeof(RoadVertexDesc), 1, fp);
		fwrite(&tgt, sizeof(RoadVertexDesc), 1, fp);
		
		unsigned int type = edge->getType();
		fwrite(&type, sizeof(unsigned int), 1, fp);

		unsigned int lanes = edge->getNumLanes();
		fwrite(&lanes, sizeof(unsigned int), 1, fp);

		unsigned int oneWay;
		if (edge->oneWay) {
			oneWay = 1;
		} else {
			oneWay = 0;
		}
		fwrite(&oneWay, sizeof(unsigned int), 1, fp);

		int nPoints = edge->getPolyLine().size();
		fwrite(&nPoints, sizeof(int), 1, fp);

		for (int i = 0; i < edge->getPolyLine().size(); i++) {
			float x = edge->getPolyLine()[i].x();
			float y = edge->getPolyLine()[i].y();
			fwrite(&x, sizeof(float), 1, fp);
			fwrite(&y, sizeof(float), 1, fp);
		}
	}
}

void RoadGraph::setWidth(float widthBase) {
	if (this->widthBase != widthBase) {
		this->widthBase = widthBase;
		modified = true;
	}
}

/**
 * エッジの重みを計算する。
 * DeadEndを繰り返し削除し、残ったやつの重みを1、削除されたやつの重みを0.1とする。
 */
void RoadGraph::computeEdgeWeights() {
	// DeadEndを削除して、重要なエッジのみを残す
	RoadGraph* temp = GraphUtil::copyRoads(this);
	GraphUtil::removeDeadEnd(temp);

	// 各エッジについて、重みを計算する
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		if (!graph[*vi]->valid) continue;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, graph); ei != eend; ++ei) {
			if (!graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, graph);

			if (GraphUtil::hasEdge(temp, *vi, tgt)) {
				graph[*ei]->weight = 1.0f;
			} else {
				graph[*ei]->weight = 0.1f;
			}
		}
	}
}

/**
 * Importanceに基づいて、エッジを並べて返却する。
 */
QList<RoadEdgeDesc> RoadGraph::getOrderedEdgesByImportance() {
	std::vector<RoadEdgeDesc> data;
	//std::vector<int> data;

	RoadEdgeIter ei, eend;
	int count = 0;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		if (!graph[*ei]->valid) continue;

		data.push_back(*ei);
		//data.push_back(count);
		count++;
	}

	std::sort(data.begin(), data.end(), MoreImportantEdge(this));

	QList<RoadEdgeDesc> ret;
	for (int i = 0; i < data.size(); i++) {
		//ret.push_back(GraphUtil::getEdge(this, data[i]));
		ret.push_back(data[i]);
	}

	return ret;
}

LessWeight::LessWeight(RoadGraph* roads) {
	this->roads = roads;
}

bool LessWeight::operator()(const RoadEdgeDesc& left, const RoadEdgeDesc& right) const {
	return roads->graph[left]->lanes < roads->graph[right]->lanes;
}

MoreImportantEdge::MoreImportantEdge(RoadGraph* roads) {
	this->roads = roads;
}

//bool MoreImportantEdge::operator()(const int& left, const int& right) const {
bool MoreImportantEdge::operator()(const RoadEdgeDesc& left, const RoadEdgeDesc& right) const {
	RoadEdge* e1 = roads->graph[left];
	RoadEdge* e2 = roads->graph[right];

	return e1->importance > e2->importance;
}