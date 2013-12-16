#pragma once

#include "RoadVertex.h"
#include "RoadEdge.h"
#include "Renderable.h"
#include <stdio.h>
#include <qvector2d.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;


typedef adjacency_list<vecS, vecS, undirectedS, RoadVertex*, RoadEdge*> BGLGraph;
typedef graph_traits<BGLGraph>::vertex_descriptor RoadVertexDesc;
typedef graph_traits<BGLGraph>::edge_descriptor RoadEdgeDesc;
typedef graph_traits<BGLGraph>::vertex_iterator RoadVertexIter;
typedef graph_traits<BGLGraph>::edge_iterator RoadEdgeIter;
typedef graph_traits<BGLGraph>::out_edge_iterator RoadOutEdgeIter;
typedef graph_traits<BGLGraph>::in_edge_iterator RoadInEdgeIter;

class CollapseAction {
public:
	RoadVertexDesc childNode;
	RoadVertexDesc parentNode;
	std::vector<RoadEdgeDesc> removedEdges;
	std::vector<RoadEdgeDesc> addedEdges;
};

class RoadGraph {
public:
	BGLGraph graph;
	bool modified;
	std::vector<Renderable> renderables;
	float widthBase;

public:
	RoadGraph();
	~RoadGraph();

	void generateMesh(float widthBase, float highwayHeight, float avenueHeight, float curbRatio, bool drawLocalStreets, float opacity = 1.0f);
	void RoadGraph::addMeshFromEdge(Renderable* renderable, RoadEdge* edge, float widthBase, QColor color, float height);

	void clear();
	void load(FILE* fp, int roadType);
	void save(FILE* fp);
	void setWidth(float widthperLane);
	void computeEdgeWeights();

	QList<RoadEdgeDesc> getOrderedEdgesByImportance();

};

class LessWeight {
private:
	RoadGraph* roads;

public:
	LessWeight(RoadGraph* roads);

	bool operator()(const RoadEdgeDesc& left, const RoadEdgeDesc& right) const;
};

class MoreImportantEdge {
private:
	RoadGraph* roads;

public:
	MoreImportantEdge(RoadGraph* roads);

	//bool operator()(const int& left, const int& right) const;
	bool operator()(const RoadEdgeDesc& left, const RoadEdgeDesc& right) const;
};
