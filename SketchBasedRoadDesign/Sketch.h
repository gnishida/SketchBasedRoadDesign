#pragma once

#include "RoadGraph.h"

class Sketch : public RoadGraph {
public:
	RoadVertexDesc curVertex;
	RoadEdgeDesc curEdge;

public:
	Sketch();
	~Sketch();

	void generateSketchMesh();
	RoadGraph* makeRoads();
};

