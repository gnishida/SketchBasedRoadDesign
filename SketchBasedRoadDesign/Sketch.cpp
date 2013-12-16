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
	renderables.clear();

	Vertex v;

	renderables.push_back(Renderable(GL_POINTS, 10.0f));
	
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		RoadEdge* edge = graph[*ei];

		renderables.push_back(Renderable(GL_LINE_STRIP, 3.0f));		
		for (int i = 0; i < edge->polyLine.size(); i++) {
			v.location[0] = edge->polyLine[i].x();
			v.location[1] = edge->polyLine[i].y();
			v.location[2] = 1.0f;
			v.color[0] = 0.0f;
			v.color[1] = 0.0f;
			v.color[2] = 1.0f;
			v.normal[0] = 0.0f;
			v.normal[1] = 0.0f;
			v.normal[2] = 1.0f;

			renderables[renderables.size() - 1].vertices.push_back(v);
		}

		// draw points along the edge poly line
		for (int i = 0; i < edge->polyLine.size(); i++) {
			v.location[0] = edge->polyLine[i].x();
			v.location[1] = edge->polyLine[i].y();
			v.location[2] = 1.0f;
			v.color[0] = 0.0f;
			v.color[1] = 0.0f;
			v.color[2] = 1.0f;
			v.normal[0] = 0.0f;
			v.normal[1] = 0.0f;
			v.normal[2] = 1.0f;

			renderables[0].vertices.push_back(v);
		}
	}
}

/**
 * Create a road graph from the sketch and return it.
 */
RoadGraph* Sketch::makeRoads() {
	RoadGraph* roads = GraphUtil::copyRoads(this);

	GraphUtil::planarify(roads);

	return roads;
}