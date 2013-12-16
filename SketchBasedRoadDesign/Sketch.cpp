#include "Sketch.h"
#include "GraphUtil.h"

Sketch::Sketch() : RoadGraph() {
}

Sketch::~Sketch() {
}

/**
 * Build a mesh according to the sketch
 */
void Sketch::generateSketchMesh() {
}

/**
 * Create a road graph from the sketch and return it.
 */
RoadGraph* Sketch::makeRoads() {
	RoadGraph* roads = GraphUtil::copyRoads(this);

	GraphUtil::planarify(roads);

	return roads;
}