﻿#include "GraphUtil.h"
#include "Util.h"
#include "Array2D.h"
#include "BFSForest.h"
#include <qlist.h>
#include <qmatrix.h>
#include <qdebug.h>

#ifndef M_PI
#define M_PI	3.141592653
#endif

EdgePair::EdgePair(RoadEdgeDesc edge1, RoadEdgeDesc edge2) {
	this->edge1 = edge1;
	this->edge2 = edge2;
}

EdgePairComparison::EdgePairComparison(RoadGraph* roads1, RoadGraph* roads2) {
	this->roads1 = roads1;
	this->roads2 = roads2;
}

/**
 * Sort the edge pairs according to the dissimilarity.
 * The pair with low dissimilarity comes to the head of the list, i.e. the head of the list contains the most similar edge pair.
 */
bool EdgePairComparison::operator()(const EdgePair& left, const EdgePair& right) const {
	float dissimilarity1 = GraphUtil::computeDissimilarityOfEdges(roads1, left.edge1, roads2, left.edge2);
	float dissimilarity2 = GraphUtil::computeDissimilarityOfEdges(roads1, right.edge1, roads2, right.edge2);

	return dissimilarity1 < dissimilarity2;
}

/**
 * Return the number of vertices.
 *
 */
int GraphUtil::getNumVertices(RoadGraph* roads, bool onlyValidVertex) {
	if (!onlyValidVertex) {
		return boost::num_vertices(roads->graph);
	}

	int count = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		count++;
	}

	return count;
}

/**
 * Return the number of vertices which are connected to the specified vertex.
 */
int GraphUtil::getNumConnectedVertices(RoadGraph* roads, RoadVertexDesc start, bool onlyValidVertex) {
	int count = 1;

	QList<RoadVertexDesc> queue;
	queue.push_back(start);

	QList<RoadVertexDesc> visited;
	visited.push_back(start);

	while (!queue.empty()) {
		RoadVertexDesc v = queue.front();
		queue.pop_front();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
			if (onlyValidVertex && !roads->graph[*ei]->valid) continue;

			RoadVertexDesc u = boost::target(*ei, roads->graph);
			if (onlyValidVertex && !roads->graph[u]->valid) continue;

			if (visited.contains(u)) continue;

			visited.push_back(u);
			queue.push_back(u);
			count++;
		}
	}

	return count;
}

/**
 * Return the index-th vertex.
 */
RoadVertexDesc GraphUtil::getVertex(RoadGraph* roads, int index, bool onlyValidVertex) {
	int count = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		if (count == index) return *vi;

		count++;
	}

	throw "Index exceeds the number of vertices.";
}

/**
 * Find the closest vertex from the specified point. 
 */
RoadVertexDesc GraphUtil::getVertex(RoadGraph* roads, QVector2D pt, bool onlyValidVertex) {
	RoadVertexDesc nearest_desc;
	float min_dist = std::numeric_limits<float>::max();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		float dist = (roads->graph[*vi]->getPt() - pt).length();
		if (dist < min_dist) {
			nearest_desc = *vi;
			min_dist = dist;
		}
	}

	return nearest_desc;

}

/**
 * Find the closest vertex from the specified point. 
 * If the closet vertex is within the threshold, return true. Otherwise, return false.
 */
bool GraphUtil::getVertex(RoadGraph* roads, QVector2D pos, float threshold, RoadVertexDesc& desc, bool onlyValidVertex) {
	float min_dist = std::numeric_limits<float>::max();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		float dist = (roads->graph[*vi]->getPt() - pos).length();
		if (dist < min_dist) {
			min_dist = dist;
			desc = *vi;
		}
	}

	if (min_dist <= threshold) return true;
	else return false;
}

/**
 * Find the closest vertex from the specified point. 
 * If the closet vertex is within the threshold, return true. Otherwise, return false.
 */
bool GraphUtil::getVertex(RoadGraph* roads, QVector2D pos, float threshold, RoadVertexDesc ignore, RoadVertexDesc& desc, bool onlyValidVertex) {
	float min_dist = std::numeric_limits<float>::max();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;
		if (*vi == ignore) continue;

		float dist = (roads->graph[*vi]->getPt() - pos).length();
		if (dist < min_dist) {
			min_dist = dist;
			desc = *vi;
		}
	}

	if (min_dist <= threshold) return true;
	else return false;
}


/**
 * 当該頂点が、何番目の頂点かを返却する。
 */
int GraphUtil::getVertexIndex(RoadGraph* roads, RoadVertexDesc desc, bool onlyValidVertex) {
	int count = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		if (*vi == desc) return count;

		count++;
	}

	throw "The specified vertex does not exist.";
}

/**
 * Add a vertex.
 */
RoadVertexDesc GraphUtil::addVertex(RoadGraph* roads, RoadVertex* v) {
	RoadVertex* new_v = new RoadVertex(*v);
	RoadVertexDesc new_v_desc = boost::add_vertex(roads->graph);
	roads->graph[new_v_desc] = new_v;

	return new_v_desc;
}

/**
 * Move the vertex to the specified location.
 * The outing edges are also moved accordingly.
 */
void GraphUtil::moveVertex(RoadGraph* roads, RoadVertexDesc v, QVector2D pt) {
	// Move the outing edges
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		std::vector<QVector2D> polyLine = roads->graph[*ei]->polyLine;
		if ((polyLine[0] - roads->graph[v]->getPt()).lengthSquared() < (polyLine[0] - roads->graph[tgt]->getPt()).lengthSquared()) {
			std::reverse(polyLine.begin(), polyLine.end());
		}

		int num = polyLine.size();
		QVector2D dir = pt - roads->graph[v]->pt;
		for (int i = 0; i < num - 1; i++) {
			polyLine[i] += dir * (float)i / (float)(num - 1);
		}
		polyLine[num - 1] = pt;

		roads->graph[*ei]->polyLine = polyLine;
	}

	// Move the vertex
	roads->graph[v]->pt = pt;
}

/**
 * Collapse v1 to v2.
 */
void GraphUtil::collapseVertex(RoadGraph* roads, RoadVertexDesc v1, RoadVertexDesc v2) {
	if (v1 == v2) return;

	roads->graph[v1]->valid = false;

	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v1, roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		// retrive the neighbor of v1
		RoadVertexDesc v1b = boost::target(*ei, roads->graph);

		// invalidate the edge between v1 and v1b
		roads->graph[*ei]->valid = false;

		if (v2 == v1b) continue;

		// If there is already an edge between v2 and v1b, don't create another edge.
		if (hasEdge(roads, v2, v1b)) continue;

		// create an edge between v2 and v1b
		addEdge(roads, v2, v1b, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);
	}
}

/**
 * Return the degree of the specified vertex.
 */
int GraphUtil::getDegree(RoadGraph* roads, RoadVertexDesc v, bool onlyValidEdge) {
	if (onlyValidEdge) {
		int count = 0;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
			if (roads->graph[*ei]->valid) count++;
		}
		return count;
	} else {
		return boost::degree(v, roads->graph);
	}
}

/**
 * Return the list of vertices.
 */
std::vector<RoadVertexDesc> GraphUtil::getVertices(RoadGraph* roads, bool onlyValidVertex) {
	std::vector<RoadVertexDesc> ret;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		ret.push_back(*vi);
	}

	return ret;
}

/**
 * Remove the isolated vertices.
 */
void GraphUtil::removeIsolatedVertices(RoadGraph* roads, bool onlyValidVertex) {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		if (getDegree(roads, *vi, onlyValidVertex) == 0) {
			roads->graph[*vi]->valid = false;
		}
	}
}

/**
 * Snap v1 to v2.
 */
void GraphUtil::snapVertex(RoadGraph* roads, RoadVertexDesc v1, RoadVertexDesc v2) {
	if (v1 == v2) return;

	moveVertex(roads, v1, roads->graph[v2]->pt);

	// Snap all the outing edges from v1
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v1, roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		// add a new edge
		addEdge(roads, v2, tgt, roads->graph[*ei]);

		// invalidate the old edge
		roads->graph[*ei]->valid = false;
	}

	// invalidate v1
	roads->graph[v1]->valid = false;
}

/**
 * Return the central vertex.
 */
RoadVertexDesc GraphUtil::getCentralVertex(RoadGraph* roads) {
	BBox box = getAABoundingBox(roads);
	return getVertex(roads, box.midPt());
}

/**
 * Return the index-th edge.
 */
RoadEdgeDesc GraphUtil::getEdge(RoadGraph* roads, int index, bool onlyValidEdge) {
	int count = 0;
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]) continue;

		if (index == count) return *ei;
		count++;
	}

	throw "No edge found for the specified index.";
}

/**
 * Return the total lengths of the edges outing from the specified vertex.
 */
float GraphUtil::getTotalEdgeLength(RoadGraph* roads, RoadVertexDesc v) {
	float ret = 0.0f;

	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		ret += roads->graph[*ei]->getLength();
	}

	return ret;
}

/**
 * Remove the edge. 
 * Remove the vertex that has smaller number of degrees.
 */
void GraphUtil::collapseEdge(RoadGraph* roads, RoadEdgeDesc e) {
	RoadVertexDesc v1 = boost::source(e, roads->graph);
	RoadVertexDesc v2 = boost::target(e, roads->graph);
	if (v1 == v2) return;

	// invalidate the edge
	roads->graph[e]->valid = false;

	if (getDegree(roads, v1) < getDegree(roads, v2)) {
		snapVertex(roads, v1, v2);
	} else {
		snapVertex(roads, v2, v1);
	}
}

/**
 * Return the number of edges.
 */
int GraphUtil::getNumEdges(RoadGraph* roads, bool onlyValidEdge) {
	int count = 0;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (roads->graph[*ei]->valid) count++;
	}

	return count;
}

/**
 * Add an edge.
 * Note: This function creates a straight line of edge.
 */
RoadEdgeDesc GraphUtil::addEdge(RoadGraph* roads, RoadVertexDesc src, RoadVertexDesc tgt, unsigned int lanes, unsigned int type, bool oneWay) {
	if (hasEdge(roads, src, tgt, false)) {
		// 既にエッジがある場合は、それを更新する
		RoadEdgeDesc edge_desc = getEdge(roads, src, tgt, false);
		roads->graph[edge_desc]->polyLine.clear();
		roads->graph[edge_desc]->addPoint(roads->graph[src]->getPt());
		roads->graph[edge_desc]->addPoint(roads->graph[tgt]->getPt());

		return edge_desc;
	} else {
		// エッジがない場合は、エッジを新規追加する
		RoadEdge* e = new RoadEdge(lanes, type, oneWay);
		e->addPoint(roads->graph[src]->getPt());
		e->addPoint(roads->graph[tgt]->getPt());

		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(src, tgt, roads->graph);
		roads->graph[edge_pair.first] = e;

		return edge_pair.first;
	}
}

/**
 * Add an edge.
 * Note: This function creates a edge which is copied from the reference edge.
 */
RoadEdgeDesc GraphUtil::addEdge(RoadGraph* roads, RoadVertexDesc src, RoadVertexDesc tgt, RoadEdge* ref_edge) {
	if (hasEdge(roads, src, tgt, false)) {
		// If there is an edge, update it instead of creating another one.
		RoadEdgeDesc edge_desc = getEdge(roads, src, tgt, false);
		*roads->graph[edge_desc] = *ref_edge;
		roads->graph[edge_desc]->valid = true;

		return edge_desc;
	} else {
		// If there is no edge, add an edge.
		RoadEdge* e = new RoadEdge(*ref_edge);
		e->valid = true;

		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(src, tgt, roads->graph);
		roads->graph[edge_pair.first] = e;

		return edge_pair.first;
	}
}

/**
 * Check if there is an edge between two vertices.
 */
bool GraphUtil::hasEdge(RoadGraph* roads, RoadVertexDesc desc1, RoadVertexDesc desc2, bool onlyValidEdge) {
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(desc1, roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		RoadVertexDesc tgt = boost::target(*ei, roads->graph);
		if (tgt == desc2) return true;
	}

	for (boost::tie(ei, eend) = boost::out_edges(desc2, roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		RoadVertexDesc tgt = boost::target(*ei, roads->graph);
		if (tgt == desc1) return true;
	}

	return false;
}

/**
 * Return the edge between src and tgt.
 */
RoadEdgeDesc GraphUtil::getEdge(RoadGraph* roads, RoadVertexDesc src, RoadVertexDesc tgt, bool onlyValidEdge) {
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(src, roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		if (boost::target(*ei, roads->graph) == tgt) return *ei;
	}

	for (boost::tie(ei, eend) = boost::out_edges(tgt, roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		if (boost::target(*ei, roads->graph) == src) return *ei;
	}

	throw "No edge found.";
}

/**
 * Sort the points of the polyline of the edge in such a way that the first point is the location of the src vertex.
 */
std::vector<QVector2D> GraphUtil::getOrderedPolyLine(RoadGraph* roads, RoadEdgeDesc e) {
	std::vector<QVector2D> ret = roads->graph[e]->getPolyLine();

	RoadVertexDesc src = boost::source(e, roads->graph);
	RoadVertexDesc tgt = boost::target(e, roads->graph);
	if ((roads->graph[src]->getPt() - roads->graph[e]->getPolyLine()[0]).length() < (roads->graph[tgt]->getPt() - roads->graph[e]->getPolyLine()[0]).length()) {
		return ret;
	} else {
		std::reverse(ret.begin(), ret.end());
		return ret;
	}
}

/**
 * Sort the points of the polyline of the edge in such a way that the first point is the location of the src vertex.
 */
void GraphUtil::orderPolyLine(RoadGraph* roads, RoadEdgeDesc e, RoadVertexDesc src) {
	RoadVertexDesc tgt;

	RoadVertexDesc s = boost::source(e, roads->graph);
	RoadVertexDesc t = boost::target(e, roads->graph);

	if (s == src) {
		tgt = t;
	} else {
		tgt = s;
	}

	// If the order is opposite, reverse the order.
	if ((roads->graph[src]->getPt() - roads->graph[e]->getPolyLine()[0]).length() > (roads->graph[tgt]->getPt() - roads->graph[e]->getPolyLine()[0]).length()) {
		std::reverse(roads->graph[e]->polyLine.begin(), roads->graph[e]->polyLine.end());
	}
}

/**
 * Move the edge to the specified location.
 */
void GraphUtil::moveEdge(RoadGraph* roads, RoadEdgeDesc e, QVector2D& src_pos, QVector2D& tgt_pos) {
	RoadVertexDesc src = boost::source(e, roads->graph);
	RoadVertexDesc tgt = boost::target(e, roads->graph);

	QVector2D src_diff = src_pos - roads->graph[src]->pt;
	QVector2D tgt_diff = tgt_pos - roads->graph[tgt]->pt;

	if ((roads->graph[e]->polyLine[0] - roads->graph[src]->pt).length() < (roads->graph[e]->polyLine[0] - roads->graph[tgt]->pt).length()) {
		int n = roads->graph[e]->polyLine.size();
		for (int i = 1; i < n - 1; i++) {
			roads->graph[e]->polyLine[i] += src_diff + (tgt_diff - src_diff) * (float)i / (float)(n - 1);
		}
		roads->graph[e]->polyLine[0] = src_pos;
		roads->graph[e]->polyLine[n - 1] = tgt_pos;
	} else {
		int n = roads->graph[e]->polyLine.size();
		for (int i = 1; i < n - 1; i++) {
			roads->graph[e]->polyLine[i] += tgt_diff + (src_diff - tgt_diff) * (float)i / (float)(n - 1);
		}
		roads->graph[e]->polyLine[0] = tgt_pos;
		roads->graph[e]->polyLine[n - 1] = src_pos;
	}
}

/**
 * Remove all the dead-end edges.
 */
bool GraphUtil::removeDeadEnd(RoadGraph* roads) {
	bool removed = false;

	bool removedOne = true;
	while (removedOne) {
		removedOne = false;
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
			if (!roads->graph[*vi]->valid) continue;

			if (getDegree(roads, *vi) == 1) {
				// invalidate all the outing edges.
				RoadOutEdgeIter ei, eend;
				for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
					roads->graph[*ei]->valid = false;
				}

				// invalidate the vertex as well.
				roads->graph[*vi]->valid = false;

				removedOne = true;
				removed = true;
			}
		}
	}

	return removed;
}

/**
 * Interpolate two polylines.
 * If the number of nodes are same, just interpolate them one by one.
 * Otherwise, discritize them into 10 points, and interpolate them one by one.
 */
std::vector<QVector2D> GraphUtil::interpolateEdges(RoadGraph* roads1, RoadEdgeDesc e1, RoadVertexDesc src1, RoadGraph* roads2, RoadEdgeDesc e2, RoadVertexDesc src2, float t) {

	orderPolyLine(roads1, e1, src1);
	orderPolyLine(roads2, e2, src2);

	std::vector<QVector2D> polyLine1 = roads1->graph[e1]->polyLine;
	std::vector<QVector2D> polyLine2 = roads2->graph[e2]->polyLine;

	std::vector<QVector2D> ret;

	int n1 = polyLine1.size();
	int n2 = polyLine2.size();

	if (n1 == n2) {
		for (int i = 0; i < n1; i++) {
			ret.push_back(polyLine1[i] * t + polyLine2[i] * (1.0f - t));
		}
	} else {
		for (float index = 0.0f; index < 1.0f; index += 0.1f) {
			// compute the location of the index-th point of the 1st polyline.
			int j1 = index * (float)(n1 - 1);

			float s1 = index - (float)j1 / (float)(n1 - 1);
			float t1 = (float)(j1 + 1) / (float)(n1 - 1) - index;

			QVector2D pt1 = polyLine1[j1] * t1 / (s1 + t1) + polyLine1[j1 + 1] * s1 / (s1 + t1);

			// compute the location of the index-th point of the 2nd polyline.
			int j2 = index * (float)(n2 - 1);

			float s2 = index - (float)j2 / (float)(n2 - 1);
			float t2 = (float)(j2 + 1) / (float)(n2 - 1) - index;

			QVector2D pt2 = polyLine2[j2] * t2 / (s2 + t2) + polyLine2[j2 + 1] * s2 / (s2 + t2);

			// interpolate
			ret.push_back(pt1 * t + pt2 * (1.0f - t));
		}

		// interpolate the last points
		ret.push_back(polyLine1[polyLine1.size() - 1] * t + polyLine2[polyLine2.size() - 1] * (1.0f - t));
	}

	return ret;
}

/**
 * Compute the importance of the edges.
 * importance = MAX( (w_length * length + w_valence * (valence1 + valence2) + w_lanes * lanes) / K, K / initial distance)
 */
void GraphUtil::computeImportanceOfEdges(RoadGraph* roads, float w_length, float w_valence, float w_lanes) {
	// compute the maximum length of the edges.
	float max_length = 0.0f;
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		float length = roads->graph[*ei]->getLength();
		if (length > max_length) max_length = length;
	}
	
	float max_importance = 0.0f;
	int count = 0;

	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		roads->graph[*ei]->importance = roads->graph[*ei]->getLength() / max_length * w_length + (getDegree(roads, src) + getDegree(roads, tgt)) * w_valence + roads->graph[*ei]->lanes * w_lanes;
	}
}

/**
 * Return the dissimilarity of two road graphs.
 *
 * dissimilarity = distance of two vertices + difference in angle of edges + difference in degrees + difference in lanes.
 */
float GraphUtil::computeDissimilarityOfEdges(RoadGraph* roads1, RoadEdgeDesc e1, RoadGraph* roads2, RoadEdgeDesc e2) {
	float w_distance = 0.001f;
	float w_angle = 1.25f;
	float w_degree = 0.3f;
	float w_lanes = 0.3f;

	RoadVertexDesc src1 = boost::source(e1, roads1->graph);
	RoadVertexDesc tgt1 = boost::target(e1, roads1->graph);

	RoadVertexDesc src2 = boost::source(e2, roads2->graph);
	RoadVertexDesc tgt2 = boost::target(e2, roads2->graph);

	// if src1 and tgt2, tgt1 and src2 are close to each other, exchange src2 and tgt2.
	if ((roads1->graph[src1]->pt - roads2->graph[src2]->pt).length() + (roads1->graph[tgt1]->pt - roads2->graph[tgt2]->pt).length() > (roads1->graph[src1]->pt - roads2->graph[tgt2]->pt).length() + (roads1->graph[tgt1]->pt - roads2->graph[src2]->pt).length()) {
		src2 = boost::target(e2, roads2->graph);
		tgt2 = boost::source(e2, roads2->graph);
	}

	// compute each factor
	float dist = (roads1->graph[src1]->pt - roads2->graph[src2]->pt).length() + (roads1->graph[tgt1]->pt - roads2->graph[tgt2]->pt).length();
	float angle = diffAngle(roads1->graph[src1]->pt - roads1->graph[tgt1]->pt, roads2->graph[src2]->pt - roads2->graph[tgt2]->pt);
	float degree = abs(getDegree(roads1, src1) - getDegree(roads2, src2)) + abs(getDegree(roads1, tgt1) - getDegree(roads2, tgt2));
	float lanes = abs((double)(roads1->graph[e1]->lanes - roads2->graph[e2]->lanes));

	return dist * w_distance + angle * w_angle + degree * w_degree + lanes * w_lanes;
}

/**
 * Remove the isolated edges.
 */
void GraphUtil::removeIsolatedEdges(RoadGraph* roads, bool onlyValidEdge) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		if (getDegree(roads, src, onlyValidEdge) == 1 && getDegree(roads, tgt, onlyValidEdge) == 1) {
			roads->graph[*ei]->valid = false;
			roads->graph[src]->valid = false;
			roads->graph[tgt]->valid = false;
		}
	}
}

/**
 * Copy the road graph.
 * Note: This function copies all the vertices and edges including the invalid ones. Thus, their IDs will be preserved.
 */
RoadGraph* GraphUtil::copyRoads(RoadGraph* roads) {
	RoadGraph* new_roads = new RoadGraph();
	
	QMap<RoadVertexDesc, RoadVertexDesc> conv;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		// Add a vertex
		RoadVertex* new_v = new RoadVertex(roads->graph[*vi]->getPt());
		new_v->valid = roads->graph[*vi]->valid;
		RoadVertexDesc new_v_desc = boost::add_vertex(new_roads->graph);
		new_roads->graph[new_v_desc] = new_v;	

		conv[*vi] = new_v_desc;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		RoadVertexDesc new_src = conv[src];
		RoadVertexDesc new_tgt = conv[tgt];

		// Add an edge
		RoadEdge* new_e = new RoadEdge(*roads->graph[*ei]);
		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(new_src, new_tgt, new_roads->graph);
		new_roads->graph[edge_pair.first] = new_e;
	}

	return new_roads;
}

/**
 * Copy the road graph.
 * Note: This function copies all the vertices and edges including the invalid ones. Thus, their IDs will be preserved.
 */
void GraphUtil::copyRoads(RoadGraph* roads1, RoadGraph* roads2) {
	roads2->clear();

	QMap<RoadVertexDesc, RoadVertexDesc> conv;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		// Add a vertex
		RoadVertex* new_v = new RoadVertex(roads1->graph[*vi]->getPt());
		new_v->valid = roads1->graph[*vi]->valid;
		RoadVertexDesc new_v_desc = boost::add_vertex(roads2->graph);
		roads2->graph[new_v_desc] = new_v;

		conv[*vi] = new_v_desc;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		RoadVertexDesc src = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

		RoadVertexDesc new_src = conv[src];
		RoadVertexDesc new_tgt = conv[tgt];

		// Add an edge
		RoadEdge* new_e = new RoadEdge(*roads1->graph[*ei]);
		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(new_src, new_tgt, roads2->graph);
		roads2->graph[edge_pair.first] = new_e;
	}
}

/**
 * Merge the 2nd road to the 1st road
 */
void GraphUtil::mergeRoads(RoadGraph* roads1, RoadGraph* roads2) {
	QMap<RoadVertexDesc, RoadVertexDesc> conv;

	// copy vertices from the 2nd road to the 1st road
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		RoadVertex* v1 = new RoadVertex(*roads2->graph[*vi]);
		RoadVertexDesc v1_desc = boost::add_vertex(roads1->graph);
		roads1->graph[v1_desc] = v1;

		conv[*vi] = v1_desc;
	}

	// copy edges from the 2nd road to the 1st road
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		RoadVertexDesc src2 = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt2 = boost::target(*ei, roads2->graph);

		RoadVertexDesc src1 = conv[src2];
		RoadVertexDesc tgt1 = conv[tgt2];

		addEdge(roads1, src1, tgt1, roads2->graph[*ei]);
	}

	// make the result to be a planer graph
	//planarify(roads1);	// temporarily comment out because this takes too much time
}

/**
 * Return the axix aligned bounding box of the road graph.
 */
BBox GraphUtil::getAABoundingBox(RoadGraph* roads) {
	BBox box;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		box.addPoint(roads->graph[*vi]->getPt());
	}

	return box;
}

/**
 * Return the bounding box of the road graph.
 * 
 * The bounding box is not necessarily aligned to X/Y-axis.
 * Algorithm: Rotate the road graph from -90 degree to 90 degree by 5 degree per step, compute the axix aligned bounding box, and find the minimum one in terms of its area.
 * The raod graph is updated to be rotated based on the bounding box in the end.
 */
BBox GraphUtil::getBoudingBox(RoadGraph* roads, float theta1, float theta2, float theta_step) {
	float min_area = std::numeric_limits<float>::max();
	float min_theta;
	BBox min_box;

	for (float theta = theta1; theta <= theta2; theta += theta_step) {
		RoadGraph* rotated_roads = copyRoads(roads);
		rotate(rotated_roads, theta);
		BBox box = getAABoundingBox(rotated_roads);
		if (box.dx() * box.dy() < min_area) {
			min_area = box.dx() * box.dy();
			min_theta = theta;
			min_box = box;
		}

		delete rotated_roads;
	}

	rotate(roads, min_theta);
	return min_box;
}

/**
 * Extract major roads from the road graph.
 * If remove is true, the extracted edges are removed from the original road graph, i.e. their "valid" flags become false.
 */
RoadGraph* GraphUtil::extractMajorRoad(RoadGraph* roads, bool remove) {
	float max_length = 0.0f;
	QList<RoadEdgeDesc> max_path;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		QList<RoadEdgeDesc> path;
		float length = extractMajorRoad(roads, *ei, path);
		if (length > max_length) {
			max_length = length;
			max_path = path;
		}
	}

	QMap<RoadVertexDesc, RoadVertexDesc> conv;

	// Generate a road graph with one main road segment.
	RoadGraph* new_roads = new RoadGraph();
	for (QList<RoadEdgeDesc>::iterator it = max_path.begin(); it != max_path.end(); ++it) {
		RoadVertexDesc src = boost::source(*it, roads->graph);
		RoadVertexDesc tgt = boost::target(*it, roads->graph);

		// Retrieve the corresponding vertex to src.
		RoadVertexDesc new_src;
		if (conv.contains(src)) {
			new_src = conv[src];
		} else {
			RoadVertex* v = new RoadVertex(roads->graph[src]->getPt());
			new_src = boost::add_vertex(new_roads->graph);
			new_roads->graph[new_src] = v;
			conv[src] = new_src;
		}

		// Retrieve the corresponding vertex to tgt.
		RoadVertexDesc new_tgt;
		if (conv.contains(tgt)) {
			new_tgt = conv[tgt];
		} else {
			RoadVertex* v = new RoadVertex(roads->graph[tgt]->getPt());
			new_tgt = boost::add_vertex(new_roads->graph);
			new_roads->graph[new_tgt] = v;
			conv[tgt] = new_tgt;
		}

		// Add an edge
		RoadEdgeDesc e = getEdge(roads, src, tgt);
		addEdge(new_roads, new_src, new_tgt, roads->graph[e]->lanes, roads->graph[e]->type, roads->graph[e]->oneWay);

		if (remove) {
			// remove the edge from the original road graph.
			roads->graph[e]->valid = false;
		}
	}

	return new_roads;
}

/**
 * Extract the longest straight road segment that starts from "root".
 */
float GraphUtil::extractMajorRoad(RoadGraph* roads, RoadEdgeDesc root, QList<RoadEdgeDesc>& path) {
	path.clear();
	path.push_back(root);

	float length = roads->graph[root]->getLength();

	RoadVertexDesc root1 = boost::source(root, roads->graph);
	RoadVertexDesc root2 = boost::target(root, roads->graph);

	QList<RoadVertexDesc> visited;
	visited.push_back(root1);
	visited.push_back(root2);

	QList<RoadVertexDesc> queue;
	queue.push_back(root1);
	queue.push_back(root2);

	QList<float> angles;
	QVector2D dir = roads->graph[root1]->getPt() - roads->graph[root2]->getPt();
	angles.push_back(atan2f(dir.y(), dir.x()));
	angles.push_back(atan2f(-dir.y(), -dir.x()));

	while (!queue.empty()) {
		RoadVertexDesc v = queue.front();
		queue.pop_front();

		float angle = angles.front();
		angles.pop_front();

		float min_angle;
		float min_diff_angle = std::numeric_limits<float>::max();
		RoadVertexDesc min_u;
		RoadEdgeDesc min_e;
		float len;

		// For each neighbor
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			RoadVertexDesc u = boost::target(*ei, roads->graph);
			if (!roads->graph[u]->valid) continue;

			// Skip the node if it is already visited.
			if (visited.contains(u)) continue;

			QVector2D dir2 = roads->graph[u]->getPt() - roads->graph[v]->getPt();
			float angle2 = atan2f(dir2.y(), dir2.x());
			float diff_angle = diffAngle(angle2, angle);
			if (diff_angle < min_diff_angle) {
				min_diff_angle = diff_angle;
				min_angle = angle2;
				min_u = u;
				min_e = *ei;
				len = roads->graph[*ei]->getLength();
			}
		}

		// If the angle is less than 20 degree, consider it as straight.
		if (min_diff_angle < M_PI * 20.0f / 180.0f) {
			path.push_back(min_e);
			length += len;

			queue.push_back(min_u);
			angles.push_back(min_angle);
			visited.push_back(min_u);
		}
	}

	return length;
}

/**
 * Return the neighbors of the specified vertex.
 */
std::vector<RoadVertexDesc> GraphUtil::getNeighbors(RoadGraph* roads, RoadVertexDesc v, bool onlyValidVertex) {
	std::vector<RoadVertexDesc> neighbors;

	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
		if (onlyValidVertex && !roads->graph[*ei]->valid) continue;

		neighbors.push_back(boost::target(*ei, roads->graph));
	}

	return neighbors;
}

bool GraphUtil::isNeighbor(RoadGraph* roads, RoadVertexDesc v1, RoadVertexDesc v2) {
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::out_edges(v1, roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		if (boost::target(*ei, roads->graph) == v2) return true;
	}
	for (boost::tie(ei, eend) = boost::out_edges(v2, roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		if (boost::target(*ei, roads->graph) == v1) return true;
	}

	return false;
}

/**
 * Check if desc2 is reachable from desc1.
 */
bool GraphUtil::isConnected(RoadGraph* roads, RoadVertexDesc desc1, RoadVertexDesc desc2, bool onlyValidEdge) {
	QList<RoadVertexDesc> seeds;
	QSet<RoadVertexDesc> visited;

	seeds.push_back(desc1);
	visited.insert(desc1);

	while (!seeds.empty()) {
		RoadVertexDesc v = seeds.front();
		seeds.pop_front();

		visited.insert(v);

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads->graph); ei != eend; ++ei) {
			if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

			RoadVertexDesc u = boost::target(*ei, roads->graph);
			if (onlyValidEdge && !roads->graph[u]->valid) continue;

			if (u == desc2) return true;

			if (!visited.contains(u)) seeds.push_back(u);			
		}
	}

	return false;
}

/**
 * Find the closest vertex from the specified point.
 */
/*RoadVertexDesc GraphUtil::findNearestVertex(RoadGraph* roads, const QVector2D &pt) {
	RoadVertexDesc nearest_desc;
	float min_dist = std::numeric_limits<float>::max();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		float dist = (roads->graph[*vi]->getPt() - pt).length();
		if (dist < min_dist) {
			nearest_desc = *vi;
			min_dist = dist;
		}
	}

	return nearest_desc;
}*/

/**
 * Find the closest vertex from the specified point.
 * The vertex "ignore" is ignored.
 */
/*RoadVertexDesc GraphUtil::findNearestVertex(RoadGraph* roads, const QVector2D &pt, RoadVertexDesc ignore) {
	RoadVertexDesc nearest_desc;
	float min_dist = std::numeric_limits<float>::max();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (*vi == ignore) continue;
		if (!roads->graph[*vi]->valid) continue;

		float dist = (roads->graph[*vi]->getPt() - pt).length();
		if (dist < min_dist) {
			nearest_desc = *vi;
			min_dist = dist;
		}
	}

	return nearest_desc;
}*/

/**
 * 指定したノードvと接続されたノードの中で、指定した座標に最も近いノードを返却する。
 */
RoadVertexDesc GraphUtil::findConnectedNearestNeighbor(RoadGraph* roads, const QVector2D &pt, RoadVertexDesc v) {
	QMap<RoadVertexDesc, bool> visited;
	std::list<RoadVertexDesc> seeds;
	seeds.push_back(v);

	float min_dist = std::numeric_limits<float>::max();
	RoadVertexDesc min_desc;

	while (!seeds.empty()) {
		RoadVertexDesc seed = seeds.front();
		seeds.pop_front();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(seed, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			RoadVertexDesc v2 = boost::target(*ei, roads->graph);
			if (visited.contains(v2)) continue;

			// 指定したノードvは除く（除かない方が良いのか？検討中。。。。）
			//if (v2 == v) continue;

			visited[v2] = true;

			// 指定した座標との距離をチェック
			float dist = (roads->graph[v2]->getPt() - pt).length();
			if (dist < min_dist) {
				min_dist = dist;
				min_desc = v2;
			}

			seeds.push_back(v2);
		}
	}

	return min_desc;
}

/**
 * Find the edge which is the closest to the specified point.
 * If the distance is within the threshold, return true. Otherwise, return false.
 */
bool GraphUtil::getEdge(RoadGraph* roads, const QVector2D &pt, float threshold, RoadEdgeDesc& e, bool onlyValidEdge) {
	float min_dist = std::numeric_limits<float>::max();
	RoadEdgeDesc min_e;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		RoadVertex* src = roads->graph[boost::source(*ei, roads->graph)];
		RoadVertex* tgt = roads->graph[boost::target(*ei, roads->graph)];

		if (onlyValidEdge && !src->valid) continue;
		if (onlyValidEdge && !tgt->valid) continue;

		QVector2D pt2;
		for (int i = 0; i < roads->graph[*ei]->polyLine.size() - 1; i++) {
			float dist = Util::pointSegmentDistanceXY(roads->graph[*ei]->polyLine[i], roads->graph[*ei]->polyLine[i + 1], pt, pt2);
			if (dist < min_dist) {
				min_dist = dist;
				e = *ei;
			}
		}
	}

	if (min_dist < threshold) return true;
	else return false;
}

/**
 * 指定された頂点に最も近いエッジを返却する。
 * ただし、指定された頂点に隣接するエッジは、対象外とする。
 */
RoadEdgeDesc GraphUtil::findNearestEdge(RoadGraph* roads, RoadVertexDesc v, float& dist, QVector2D &closestPt, bool onlyValidEdge) {
	dist = std::numeric_limits<float>::max();
	RoadEdgeDesc min_e;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (onlyValidEdge && !roads->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);
		if (v == src || v == tgt) continue;

		if (onlyValidEdge && !roads->graph[src]->valid) continue;
		if (onlyValidEdge && !roads->graph[tgt]->valid) continue;

		QVector2D pt2;
		float d = Util::pointSegmentDistanceXY(roads->graph[src]->getPt(), roads->graph[tgt]->getPt(), roads->graph[v]->getPt(), pt2);
		if (d < dist) {
			dist = d;
			min_e = *ei;
			closestPt = pt2;
		}
	}

	return min_e;
}

/**
 * Clean the road graph by removing all the invalid vertices and edges.
 */
void GraphUtil::clean(RoadGraph* roads) {
	RoadGraph* temp = GraphUtil::copyRoads(roads);

	roads->clear();

	QMap<RoadVertexDesc, RoadVertexDesc> conv;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(temp->graph); vi != vend; ++vi) {
		if (!temp->graph[*vi]->valid) continue;

		// Add a vertex
		RoadVertex* new_v = new RoadVertex(temp->graph[*vi]->getPt());
		RoadVertexDesc new_v_desc = boost::add_vertex(roads->graph);
		roads->graph[new_v_desc] = new_v;	

		conv[*vi] = new_v_desc;
	}

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(temp->graph); ei != eend; ++ei) {
		if (!temp->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, temp->graph);
		RoadVertexDesc tgt = boost::target(*ei, temp->graph);

		RoadVertexDesc new_src = conv[src];
		RoadVertexDesc new_tgt = conv[tgt];

		// Add an edge
		RoadEdge* new_e = new RoadEdge(*temp->graph[*ei]);
		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(new_src, new_tgt, roads->graph);
		roads->graph[edge_pair.first] = new_e;
	}
}

/**
 * Remove the vertices of degree of 2, and make it as a part of an edge.
 */
void GraphUtil::reduce(RoadGraph* roads) {
	// reduce the graph by removing the vertices which have two outing edges.
	RoadVertexIter vi, vend;
	bool deleted = false;
	do {
		deleted = false;

		for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
			if (!roads->graph[*vi]->valid) continue;

			RoadVertex* v = roads->graph[*vi];

			if (getDegree(roads, *vi) == 2) {
				if (reduce(roads, *vi)) {
					deleted = true;
					break;
				}
			}
		}
	} while (deleted);
}

/**
 * Remove the vertex of degree 2, and make it as a part of an edge.
 */
bool GraphUtil::reduce(RoadGraph* roads, RoadVertexDesc desc) {
	int count = 0;
	RoadVertexDesc vd[2];
	RoadEdgeDesc ed[2];
	RoadEdge* edges[2];

	RoadOutEdgeIter ei, ei_end;
	for (boost::tie(ei, ei_end) = out_edges(desc, roads->graph); ei != ei_end; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		vd[count] = boost::target(*ei, roads->graph);
		ed[count] = *ei;
		edges[count] = roads->graph[*ei];
		count++;
	}

	//if (edges[0]->getType() != edges[1]->getType()) return false;
	//if (edges[0]->lanes != edges[1]->lanes) return false;

	// If the vertices form a triangle, don't remove it.
	if (hasEdge(roads, vd[0], vd[1])) return false;

	RoadEdge* new_edge = new RoadEdge(edges[0]->oneWay, edges[0]->lanes, edges[0]->type);
	orderPolyLine(roads, ed[0], vd[0]);
	orderPolyLine(roads, ed[1], desc);
	
	for (int i = 0; i < edges[0]->getPolyLine().size(); i++) {
		new_edge->addPoint(edges[0]->getPolyLine()[i]);
	}
	for (int i = 1; i < edges[1]->getPolyLine().size(); i++) {
		new_edge->addPoint(edges[1]->getPolyLine()[i]);
	}
	std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(vd[0], vd[1], roads->graph);
	roads->graph[edge_pair.first] = new_edge;

	// invalidate the old edge
	roads->graph[ed[0]]->valid = false;
	roads->graph[ed[1]]->valid = false;

	// invalidate the vertex
	roads->graph[desc]->valid = false;

	return true;
}

/**
 * ノード間の距離が指定した距離よりも近い場合は、１つにしてしまう。
 * ノードとエッジ間の距離が、閾値よりも小さい場合も、エッジ上にノードを移してしまう。
 */
void GraphUtil::simplify(RoadGraph* roads, float dist_threshold) {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		while (true) {
			//RoadVertexDesc v2 = findNearestVertex(roads, roads->graph[*vi]->getPt(), *vi);
			RoadVertexDesc v2;
			if (!getVertex(roads, roads->graph[*vi]->getPt(), dist_threshold, *vi, v2)) break;
			//if ((roads->graph[v2]->getPt() - roads->graph[*vi]->getPt()).length() > dist_threshold) break;

			QVector2D pt = (roads->graph[*vi]->pt + roads->graph[v2]->pt) / 2.0f;

			collapseVertex(roads, v2, *vi);
			roads->graph[*vi]->pt = pt;
		}

		// find the closest vertex
		QVector2D closestPt;
		float dist;
		RoadEdgeDesc e = GraphUtil::findNearestEdge(roads, *vi, dist, closestPt);
		if (dist < dist_threshold) {
			// move the vertex to the closest point on the edge
			GraphUtil::moveVertex(roads, *vi, closestPt);

			// retrieve src and tgt of the edge
			RoadVertexDesc src = boost::source(e, roads->graph);
			RoadVertexDesc tgt = boost::target(e, roads->graph);

			// invalidate the edge
			roads->graph[e]->valid = false;

			// update the edge
			if (!GraphUtil::hasEdge(roads, src, *vi)) {
				addEdge(roads, src, *vi, roads->graph[e]->lanes, roads->graph[e]->type, roads->graph[e]->oneWay);
			}
			if (!GraphUtil::hasEdge(roads, tgt, *vi)) {
				addEdge(roads, tgt, *vi, roads->graph[e]->lanes, roads->graph[e]->type, roads->graph[e]->oneWay);
			}
		}
	}
}

/**
 * エッジのポリゴンが3つ以上で構成されている場合、中間点を全てノードとして登録する。
 */
void GraphUtil::normalize(RoadGraph* roads) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		if (roads->graph[*ei]->getPolyLine().size() <= 2) continue;

		// invalidate the edge
		roads->graph[*ei]->valid = false;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		RoadVertexDesc prev_desc;
		RoadVertexDesc last_desc;
		if ((roads->graph[src]->getPt() - roads->graph[*ei]->getPolyLine()[0]).length() < (roads->graph[tgt]->getPt() - roads->graph[*ei]->getPolyLine()[0]).length()) {
			prev_desc = src;
			last_desc = tgt;
		} else {
			prev_desc = tgt;
			last_desc = src;
		}

		for (int i = 1; i < roads->graph[*ei]->getPolyLine().size() - 1; i++) {
			// add all the points along the poly line as vertices
			RoadVertex* new_v = new RoadVertex(roads->graph[*ei]->getPolyLine()[i]);
			RoadVertexDesc new_v_desc = boost::add_vertex(roads->graph);
			roads->graph[new_v_desc] = new_v;

			// Add an edge
			addEdge(roads, prev_desc, new_v_desc, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);

			prev_desc = new_v_desc;
		}

		// Add the last edge
		addEdge(roads, prev_desc, last_desc, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);
	}
}

/**
 * start頂点と接続されているノードのみ有効とし、それ以外のノード、およびエッジを、全て無効にする。
 * 本実装では、事前の有効・無効フラグを考慮していない。要検討。。。
 */
void GraphUtil::singlify(RoadGraph* roads) {
	int max_size = 0;
	RoadVertexDesc start;

	// 最も大きいかたまり（接続されている）の道路網を探し出す
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		int size = getNumConnectedVertices(roads, *vi);
		if (size > max_size) {
			max_size = size;
			start = *vi;
		}
	}

	RoadGraph* new_roads = new RoadGraph();

	std::list<RoadVertexDesc> queue;
	queue.push_back(start);

	QMap<RoadVertexDesc, RoadVertexDesc> conv;

	// Add the starting vertex
	RoadVertex* new_v = new RoadVertex(roads->graph[start]->getPt());
	RoadVertexDesc new_v_desc = boost::add_vertex(new_roads->graph);
	new_roads->graph[new_v_desc] = new_v;

	conv[start] = new_v_desc;

	std::list<RoadVertexDesc> queue2;
	queue2.push_back(new_v_desc);

	while (!queue.empty()) {
		RoadVertexDesc v_desc = queue.front();
		queue.pop_front();

		RoadVertexDesc new_v_desc = queue2.front();
		queue2.pop_front();


		RoadOutEdgeIter oei, oeend;
		for (boost::tie(oei, oeend) = boost::out_edges(v_desc, roads->graph); oei != oeend; ++oei) {
			if (!roads->graph[*oei]->valid) continue;

			RoadVertexDesc u_desc = boost::target(*oei, roads->graph);
			if (!roads->graph[u_desc]->valid) continue;

			RoadVertexDesc new_u_desc;

			if (conv.contains(u_desc)) {
				new_u_desc = conv[u_desc];
			} else {
				// Add a vertex
				RoadVertex* new_u = new RoadVertex(roads->graph[u_desc]->getPt());
				new_u_desc = boost::add_vertex(new_roads->graph);
				new_roads->graph[new_u_desc] = new_u;
			}

			// Add an edge
			if (!hasEdge(new_roads, new_v_desc, new_u_desc)) {
				addEdge(new_roads, new_v_desc, new_u_desc, roads->graph[*oei]);
			}

			if (!conv.contains(u_desc)) {
				conv[u_desc] = new_u_desc;
				queue.push_back(u_desc);
				queue2.push_back(new_u_desc);
			}
		}
	}

	// copy new_roads to roads
	copyRoads(new_roads, roads);
	delete new_roads;
}

/**
 * Convert the road graph to a planar graph.
 */
void GraphUtil::planarify(RoadGraph* roads) {
	bool split = true;

	while (split) {
		split = planarifyOne(roads);
	}
}

/**
 * Convert one intersected road segments to a planar one by adding the intersection, and return true.
 * If the road segments do not intersect, return false.
 */
bool GraphUtil::planarifyOne(RoadGraph* roads) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		RoadEdge* e = roads->graph[*ei];
		if (!e->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		RoadEdgeIter ei2, eend2;
		for (boost::tie(ei2, eend2) = boost::edges(roads->graph); ei2 != eend2; ++ei2) {
			RoadEdge* e2 = roads->graph[*ei2];
			if (!e2->valid) continue;

			RoadVertexDesc src2 = boost::source(*ei2, roads->graph);
			RoadVertexDesc tgt2 = boost::target(*ei2, roads->graph);

			//if ((src == src2 && tgt == tgt2) || (src == tgt2 && tgt == src2)) continue;
			if (src == src2 || src == tgt2 || tgt == src2 || tgt == tgt2) continue;

			for (int i = 0; i < e->polyLine.size() - 1; i++) {
				for (int j = 0; j < e2->polyLine.size() - 1; j++) {
					float tab, tcd;
					QVector2D intPt;
					if (Util::segmentSegmentIntersectXY(e->polyLine[i], e->polyLine[i+1], e2->polyLine[j], e2->polyLine[j+1], &tab, &tcd, true, intPt)) {
						// エッジの端、ぎりぎりで、交差する場合は、交差させない
						if ((roads->graph[src]->pt - intPt).length() < 10 || (roads->graph[tgt]->pt - intPt).length() < 10 || (roads->graph[src2]->pt - intPt).length() < 10 || (roads->graph[tgt2]->pt - intPt).length() < 10) continue;

						// 交点をノードとして登録
						RoadVertex* new_v = new RoadVertex(intPt);
						RoadVertexDesc new_v_desc = boost::add_vertex(roads->graph);
						roads->graph[new_v_desc] = new_v;

						// もともとのエッジを無効にする
						roads->graph[*ei]->valid = false;
						roads->graph[*ei2]->valid = false;

						// 新たなエッジを追加する
						addEdge(roads, src, new_v_desc, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);
						addEdge(roads, new_v_desc, tgt, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);

						addEdge(roads, src2, new_v_desc, roads->graph[*ei2]->lanes, roads->graph[*ei2]->type, roads->graph[*ei2]->oneWay);
						addEdge(roads, new_v_desc, tgt2, roads->graph[*ei2]->lanes, roads->graph[*ei2]->type, roads->graph[*ei2]->oneWay);

						return true;
					}
				}
			}
		}
	}

	return false;
}

/**
 * 道路網をスケルトン化する。
 * 具体的には、オリジナル道路網で、degreeが1の頂点と、その隣接エッジを無効にする。
 * 注意：頂点を削除した結果、新たにdegreeが1となる頂点は、その対象ではない。
 */
void GraphUtil::skeltonize(RoadGraph* roads) {
	QList<RoadVertexDesc> list;

	// 削除対象となる頂点リストを取得
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		if (getDegree(roads, *vi) == 1) {
			list.push_back(*vi);
		}
	}

	for (int i = 0; i < list.size(); i++) {
		// 隣接エッジを無効にする
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(list[i], roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			roads->graph[*ei]->valid = false;
		}

		// 頂点を無効にする
		roads->graph[list[i]]->valid = false;
	}
}

/**
 * Rotate the road graph by theta [rad].
 */
void GraphUtil::rotate(RoadGraph* roads, float theta) {
	// Rotate vertices
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		QVector2D pos = roads->graph[*vi]->pt;

		roads->graph[*vi]->pt.setX(cosf(theta) * pos.x() - sinf(theta) * pos.y());
		roads->graph[*vi]->pt.setY(sinf(theta) * pos.x() + cosf(theta) * pos.y());
	}

	// Rotate edges
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			QVector2D pos = roads->graph[*ei]->polyLine[i];
			roads->graph[*ei]->polyLine[i].setX(cosf(theta) * pos.x() - sinf(theta) * pos.y());
			roads->graph[*ei]->polyLine[i].setY(sinf(theta) * pos.x() + cosf(theta) * pos.y());
		}
	}
}

/**
 * Translate the road graph.
 */
void GraphUtil::translate(RoadGraph* roads, QVector2D offset) {
	// Translate vertices
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		roads->graph[*vi]->pt += offset;
	}

	// Translate edges
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			roads->graph[*ei]->polyLine[i] += offset;
		}
	}
}

/**
 * 道路網をグリッド型に無理やり変換する。
 * 頂点startから開始し、エッジの方向に基づいて、上下左右方向に、ノードを広げていくイメージ。
 */
RoadGraph* GraphUtil::convertToGridNetwork(RoadGraph* roads, RoadVertexDesc start) {
 	RoadGraph* new_roads = new RoadGraph();

	QList<RoadVertexDesc> queue;
	queue.push_back(start);

	QList<RoadVertexDesc> visited;
	visited.push_back(start);

	// スタート頂点を追加
	RoadVertex* v = new RoadVertex(QVector2D(0, 0));
	RoadVertexDesc v_desc = boost::add_vertex(new_roads->graph);
	new_roads->graph[v_desc] = v;
	
	QList<RoadVertexDesc> new_queue;
	new_queue.push_back(v_desc);

	while (!queue.empty()) {
		RoadVertexDesc v_desc = queue.front();
		queue.pop_front();
		RoadVertexDesc new_v_desc = new_queue.front();
		new_queue.pop_front();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v_desc, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			RoadVertexDesc u_desc = boost::target(*ei, roads->graph);
			if (!roads->graph[u_desc]->valid) continue;

			// オリジナルの道路網で、エッジの方向を取得
			QVector2D dir = roads->graph[u_desc]->getPt() - roads->graph[v_desc]->getPt();

			QVector2D pos;
			if (diffAngle(dir, QVector2D(1, 0)) < M_PI * 0.25f) { // X軸正方向
				pos = new_roads->graph[new_v_desc]->getPt() + QVector2D(100.0f, 0.0f);
			} else if (diffAngle(dir, QVector2D(0, 1)) < M_PI * 0.25f) { // Y軸正方向
				pos = new_roads->graph[new_v_desc]->getPt() + QVector2D(0.0f, 100.0f);
			} else if (diffAngle(dir, QVector2D(-1, 0)) < M_PI * 0.25f) { // X軸負方向
				pos = new_roads->graph[new_v_desc]->getPt() + QVector2D(-100.0f, 0.0f);
			} else if (diffAngle(dir, QVector2D(0, -1)) < M_PI * 0.25f) { // Y軸負方向
				pos = new_roads->graph[new_v_desc]->getPt() + QVector2D(0.0f, -100.0f);
			} 
			
			RoadVertexDesc new_u_desc;
			if (!getVertex(new_roads, pos, 0.0f, new_u_desc)) {
				// 頂点を追加
				RoadVertex* new_u = new RoadVertex(pos);
				new_u_desc = boost::add_vertex(new_roads->graph);
				new_roads->graph[new_u_desc] = new_u;
			}

			if (!hasEdge(new_roads, new_v_desc, new_u_desc)) {
				// エッジを追加
				addEdge(new_roads, new_v_desc, new_u_desc, roads->graph[*ei]->lanes, roads->graph[*ei]->type, roads->graph[*ei]->oneWay);
			}

			if (!visited.contains(u_desc)) {
				visited.push_back(u_desc);

				queue.push_back(u_desc);
				new_queue.push_back(new_u_desc);
			}
		}
	}

	return new_roads;
}

/**
 * 道路網をグリッド型のネットワークで近似する。
 * 11/23に実装。急ぎで実装し、ほとんどテストしていない。バグがある可能性大。
 * 
 * @param cellLength		１つのセルの１辺の長さ
 * @param orig				原点の座標
 */
RoadGraph* GraphUtil::approximateToGridNetwork(RoadGraph* roads, float cellLength, QVector2D orig) {
	Array2D<RoadVertexDesc> grid_desc;
	Array2D<float> grid_weight;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		int u = (roads->graph[*vi]->getPt().x() - orig.x()) / cellLength;
		int v = (roads->graph[*vi]->getPt().y() - orig.y()) / cellLength;

		if (u < 0 || v < 0) continue;

		// 当該頂点の重要度を計算する
		float weight = 0.0f;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			weight += roads->graph[*ei]->getLength() * roads->graph[*ei]->lanes;
		}

		if (weight > grid_weight[u][v]) {
			grid_weight[u][v] = weight;
			grid_desc[u][v] = *vi;
		}
	}

	// グリッド情報に基づいて、道路網を作成する
	RoadGraph* new_roads = new RoadGraph();
	QMap<RoadVertexDesc, RoadVertexDesc> conv;

	// まずは、頂点のみを作成
	for (int i = 0; i < grid_desc.size(); i++) {
		for (int j = 0; j < grid_desc[i].size(); j++) {
			if (grid_weight[i][j] == 0.0f) continue;

			RoadVertex* v = new RoadVertex(orig + QVector2D(cellLength, 0) * j + QVector2D(0, cellLength) * i);
			RoadVertexDesc v_desc = boost::add_vertex(new_roads->graph);
			new_roads->graph[v_desc] = v;

			conv[grid_desc[i][j]] = v_desc;
		}
	}

	// 次に、エッジを作成
	for (int i = 0; i < grid_desc.size(); i++) {
		for (int j = 0; j < grid_desc[i].size(); j++) {
			if (grid_weight[i][j] == 0.0f) continue;

			if (j < grid_desc[i].size() - 1) {
				// 右隣とのエッジをチェック
				if (grid_weight[i][j+1] > 0.0f) {
					RoadVertexDesc orig_v1_desc = grid_desc[i][j];
					RoadVertexDesc orig_v2_desc = grid_desc[i][j+1];
					if (isConnected(roads, orig_v1_desc, orig_v2_desc)) {
						addEdge(new_roads, conv[orig_v1_desc], conv[orig_v2_desc], 2, 2);	// to be updated!
					}
				}
			}

			if (j > 0) {
				// 左隣とのエッジをチェック
				if (grid_weight[i][j-1] > 0.0f) {
					RoadVertexDesc orig_v1_desc = grid_desc[i][j];
					RoadVertexDesc orig_v2_desc = grid_desc[i][j-1];
					if (isConnected(roads, orig_v1_desc, orig_v2_desc)) {
						addEdge(new_roads, conv[orig_v1_desc], conv[orig_v2_desc], 2, 2);	// to be updated!
					}
				}
			}

			if (i < grid_desc.size()) {
				// 上隣とのエッジをチェック
				if (grid_weight[i+1][j] > 0.0f) {
					RoadVertexDesc orig_v1_desc = grid_desc[i][j];
					RoadVertexDesc orig_v2_desc = grid_desc[i+1][j];
					if (isConnected(roads, orig_v1_desc, orig_v2_desc)) {
						addEdge(new_roads, conv[orig_v1_desc], conv[orig_v2_desc], 2, 2);	// to be updated!
					}
				}
			}

			if (i > 0) {
				// 下隣とのエッジをチェック
				if (grid_weight[i-1][j] > 0.0f) {
					RoadVertexDesc orig_v1_desc = grid_desc[i][j];
					RoadVertexDesc orig_v2_desc = grid_desc[i-1][j];
					if (isConnected(roads, orig_v1_desc, orig_v2_desc)) {
						addEdge(new_roads, conv[orig_v1_desc], conv[orig_v2_desc], 2, 2);	// to be updated!
					}
				}
			}
		}
	}

	return new_roads;
}

/**
 * 道路網が指定されたareaにおさまるようにする。
 */
void GraphUtil::scaleToBBox(RoadGraph* roads, BBox& area) {
	BBox curArea = getAABoundingBox(roads);
	QVector2D scale(area.dx() / curArea.dx(), area.dy() / curArea.dy());

	// 全ての頂点を、指定したareaに入るよう移動する
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		QVector2D pos = roads->graph[*vi]->pt - curArea.midPt();
		pos.setX(pos.x() * scale.x());
		pos.setY(pos.y() * scale.y());
		pos += area.midPt();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, roads->graph);
			if (!roads->graph[tgt]->valid) continue;

			moveEdge(roads, *ei, pos, roads->graph[tgt]->pt);
		}

		roads->graph[*vi]->pt = pos;
	}
}

/**
 * バネの原理を使って、道路網のエッジの長さを均等にする。
 *
 * まず、全エッジの平均長を計算し、これをエッジの本来の長さと仮定する。
 * 次に、各頂点について、各隣接エッジの長さの、本来長からの変形量を使って、各頂点にかかる仮想的な力を計算する。
 * 最後に、この仮想的な力に、適当なdTをかけた値を使って、各頂点を移動させる。
 * これを、一定数、繰り返す。（終了条件について、要検討）
 */
void GraphUtil::normalizeBySpring(RoadGraph* roads, BBox& area) {
	// バネの原理を使って、各エッジの長さを均等にする
	float step = 0.03f;

	//for (int i = 0; i < 1000; i++) {
	for (int i = 0; i < 1; i++) {
		float avg_edge_length = computeAvgEdgeLength(roads);

		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
			if (!roads->graph[*vi]->valid) continue;

			// 頂点にかかる力を計算する
			QVector2D force;

			// 隣接エッジからは、引っ張られる
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
				if (!roads->graph[*ei]->valid) continue;

				RoadVertexDesc src = *vi;
				RoadVertexDesc tgt = boost::target(*ei, roads->graph);

				QVector2D dir = roads->graph[tgt]->pt - roads->graph[src]->pt;
				float x = (roads->graph[tgt]->pt - roads->graph[src]->pt).length() - avg_edge_length * 1.0f;
				
				force += dir.normalized() * x;
			}

			// 接続されていない、近隣の頂点からは、反発力を受ける
			RoadVertexIter vi2, vend2;
			for (boost::tie(vi2, vend2) = boost::vertices(roads->graph); vi2 != vend2; ++vi2) {
				if (!roads->graph[*vi2]->valid) continue;

				if (hasEdge(roads, *vi, *vi2)) continue;

				/*
				QVector2D dir = roads->graph[*vi]->pt - roads->graph[*vi2]->pt;
				float x = avg_edge_length * 1.44f - (roads->graph[*vi2]->pt - roads->graph[*vi]->pt).length();

				if (x > 0.0f) {
					force += dir.normalized() * x;
				}*/

				RoadVertexDesc src = *vi;
				RoadVertexDesc tgt = *vi2;

				QVector2D dir = roads->graph[tgt]->pt - roads->graph[src]->pt;
				float x = (roads->graph[tgt]->pt - roads->graph[src]->pt).length() - avg_edge_length * 1.0f;
				
				force += dir.normalized() * x * 0.02f;	// ←　この係数は、微調整が必要。。。
			}

			// 移動後の位置が、指定された範囲内か、チェック
			QVector2D pos = roads->graph[*vi]->pt + force * step;
			if (area.contains(pos)) {
				// 頂点をする
				roads->graph[*vi]->pt = pos;

				// エッジも移動する
				for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
					if (!roads->graph[*ei]->valid) continue;

					RoadVertexDesc tgt = boost::target(*ei, roads->graph);

					moveEdge(roads, *ei, pos, roads->graph[tgt]->pt);
				}
			}
		}
	}
}

/**
 * Remove duplicated edges if there are more than one edges between two vertices.
 */
bool GraphUtil::removeDuplicateEdges(RoadGraph* roads) {
	bool removed = false;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		QList<RoadVertexDesc> targets;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, roads->graph);

			if (targets.contains(tgt)) {
				roads->graph[*ei]->valid = false;
				removed = true;
			} else {
				targets.push_back(tgt);
			}
		}
	}
	
	return removed;
}

/**
 * snap the dead-end edges to the near vertices.
 * First, for vertices of degree more than 1, find the closest vertex.
 * If no such vertex exists, for vertices of degree 1, find the cloest vertex.
 */
void GraphUtil::snapDeadendEdges(RoadGraph* roads, float threshold) {
	float min_angle_threshold = 0.34f;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		// only for the vertices of degree more than 1
		if (GraphUtil::getDegree(roads, *vi) != 1) continue;

		// retrieve the tgt vertex
		RoadVertexDesc tgt;
		RoadEdgeDesc e_desc;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			tgt = boost::target(*ei, roads->graph);
			e_desc = *ei;
			break;
		}

		// find the closest vertex
		RoadVertexDesc nearest_desc;
		float min_dist = std::numeric_limits<float>::max();

		RoadVertexIter vi2, vend2;
		for (boost::tie(vi2, vend2) = boost::vertices(roads->graph); vi2 != vend2; ++vi2) {
			if (!roads->graph[*vi2]->valid) continue;
			if (*vi2 == *vi) continue;
			if (*vi2 == tgt) continue;
			if (GraphUtil::getDegree(roads, *vi2) == 1) continue;

			float dist = (roads->graph[*vi2]->pt - roads->graph[*vi]->pt).length();

			// 近接頂点が、*viよりもtgtの方に近い場合は、当該近接頂点は対象からはずす
			float dist2 = (roads->graph[*vi2]->pt - roads->graph[tgt]->pt).length();
			if (dist > dist2) continue;

			if (dist < min_dist) {
				nearest_desc = *vi2;
				min_dist = dist;
			}

			// *vi2から出るエッジとのなす角度の最小値が小さすぎる場合は、対象からはずす
			float min_angle = std::numeric_limits<float>::max();
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi2, roads->graph); ei != eend; ++ei) {
				if (!roads->graph[*ei]->valid) continue;

				RoadVertexDesc tgt2 = boost::target(*ei, roads->graph);
				float angle = GraphUtil::diffAngle(roads->graph[*vi]->pt - roads->graph[tgt]->pt, roads->graph[*vi2]->pt - roads->graph[tgt2]->pt);
				if (angle < min_angle) {
					min_angle = angle;
				}
			}
			if (min_angle < min_angle_threshold) continue;
		}

		// If no such vertex exists, find the closest vertex of degree 1.
		if (min_dist > threshold) {
			for (boost::tie(vi2, vend2) = boost::vertices(roads->graph); vi2 != vend2; ++vi2) {
				if (!roads->graph[*vi2]->valid) continue;
				if (*vi2 == *vi) continue;
				if (*vi2 == tgt) continue;
				if (GraphUtil::getDegree(roads, *vi2) != 1) continue;

				// Find the edge of the vertex
				RoadEdgeDesc e_desc2;
				RoadOutEdgeIter ei, eend;
				for (boost::tie(ei, eend) = boost::out_edges(*vi2, roads->graph); ei != eend; ++ei) {
					if (!roads->graph[*ei]->valid) continue;

					e_desc2 = *ei;
					break;
				}

				// If th edge is too short, skip it.
				//if (roads->graph[e_desc2]->getLength() < deadend_removal_threshold) continue;

				float dist = (roads->graph[*vi2]->pt - roads->graph[*vi]->pt).length();

				// 近接頂点が、*viよりもtgtの方に近い場合は、当該近接頂点は対象からはずす
				float dist2 = (roads->graph[*vi2]->pt - roads->graph[tgt]->pt).length();
				if (dist > dist2) continue;

				if (dist < min_dist) {
					nearest_desc = *vi2;
					min_dist = dist;
				}

				// *vi2から出るエッジとのなす角度の最小値が小さすぎる場合は、対象からはずす
				float min_angle = std::numeric_limits<float>::max();
				for (boost::tie(ei, eend) = boost::out_edges(*vi2, roads->graph); ei != eend; ++ei) {
					if (!roads->graph[*ei]->valid) continue;

					RoadVertexDesc tgt2 = boost::target(*ei, roads->graph);
					float angle = GraphUtil::diffAngle(roads->graph[*vi]->pt - roads->graph[tgt]->pt, roads->graph[*vi2]->pt - roads->graph[tgt2]->pt);
					if (angle < min_angle) {
						min_angle = angle;
					}
				}
				if (min_angle < min_angle_threshold) continue;
			}
		}

		// 当該頂点と近接頂点との距離が、snap_deadend_threshold未満か？
		if (min_dist <= threshold) {
			// 一旦、古いエッジを、近接頂点にスナップするよう移動する
			GraphUtil::moveEdge(roads, e_desc, roads->graph[nearest_desc]->pt, roads->graph[tgt]->pt);

			if (GraphUtil::hasEdge(roads, nearest_desc, tgt, false)) {
				// もともとエッジがあるが無効となっている場合、それを有効にし、エッジのポリラインを更新する
				RoadEdgeDesc new_e_desc = GraphUtil::getEdge(roads, nearest_desc, tgt, false);
				roads->graph[new_e_desc]->valid = true;
				roads->graph[new_e_desc]->polyLine = roads->graph[e_desc]->polyLine;
			} else {
				// 該当頂点間にエッジがない場合は、新しいエッジを追加する
				GraphUtil::addEdge(roads, nearest_desc, tgt, roads->graph[e_desc]);
			}

			// 古いエッジを無効にする
			roads->graph[e_desc]->valid = false;

			// 当該頂点を無効にする
			roads->graph[*vi]->valid = false;
		}
	}
}

/**
 * 指定されたdegreeの頂点について、近くに頂点がある場合は、Snapさせる。
 * ただし、Snap対象となるエッジとのなす角度がmin_angle_threshold以下の場合は、対象外。
 */
void GraphUtil::snapDeadendEdges2(RoadGraph* roads, int degree, float threshold) {
	float angle_threshold = 0.34f;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		// 指定されたdegree以外の頂点は、対象外
		if (GraphUtil::getDegree(roads, *vi) != degree) continue;

		// 当該頂点と接続されている唯一の頂点を取得
		RoadVertexDesc tgt;
		RoadEdgeDesc e_desc;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;

			tgt = boost::target(*ei, roads->graph);
			e_desc = *ei;
			break;
		}

		// 近接頂点を探す
		RoadVertexDesc nearest_desc;
		float min_dist = std::numeric_limits<float>::max();

		RoadVertexIter vi2, vend2;
		for (boost::tie(vi2, vend2) = boost::vertices(roads->graph); vi2 != vend2; ++vi2) {
			if (!roads->graph[*vi2]->valid) continue;
			if (*vi2 == *vi) continue;
			if (*vi2 == tgt) continue;

			float dist = (roads->graph[*vi2]->pt - roads->graph[*vi]->pt).length();

			// 近接頂点が、*viよりもtgtの方に近い場合は、当該近接頂点は対象からはずす
			//float dist2 = (roads->graph[*vi2]->pt - roads->graph[tgt]->pt).length();
			//if (dist > dist2) continue;

			if (dist < min_dist) {
				nearest_desc = *vi2;
				min_dist = dist;
			}
		}
		
		// 近接頂点が、*viよりもtgtの方に近い場合は、スナップしない
		if ((roads->graph[nearest_desc]->pt - roads->graph[tgt]->pt).length() < (roads->graph[*vi]->pt - roads->graph[tgt]->pt).length()) continue;

		// スナップによるエッジの角度変化が大きすぎる場合は、対象からはずす
		float diff_angle = diffAngle(roads->graph[*vi]->pt - roads->graph[tgt]->pt, roads->graph[nearest_desc]->pt - roads->graph[tgt]->pt);
		if (diff_angle > angle_threshold) continue;

		// tgtとスナップ先との間に既にエッジがある場合は、スナップしない
		if (hasEdge(roads, tgt, nearest_desc)) continue;

		// 当該頂点と近接頂点との距離が、threshold以下なら、スナップする
		if (min_dist <= threshold) {
			snapVertex(roads, *vi, nearest_desc);
		}
	}
}

/**
 * Remove too short dead-end edges unless it has a pair.
 */
void GraphUtil::removeShortDeadend(RoadGraph* roads, float threshold) {
	bool deleted = true;
	while (deleted) {
		deleted = false;

		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
			if (!roads->graph[*vi]->valid) continue;

			if (getDegree(roads, *vi) > 1) continue;

			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads->graph); ei != eend; ++ei) {
				if (!roads->graph[*ei]->valid) continue;

				// If the edge has a pair, don't remove it.
				if (roads->graph[*ei]->fullyPaired) continue;

				RoadVertexDesc tgt = boost::target(*ei, roads->graph);

				// invalidate the too short edge, and invalidate the dead-end vertex.
				if (roads->graph[*ei]->getLength() < threshold) {
					roads->graph[*vi]->valid = false;
					roads->graph[*ei]->valid = false;
					deleted = true;
				}
			}
		}
	}
}

/**
 * ２つのデータリストの差の最小値を返却する。
 * 各データは、１回のみ比較に使用できる。
 */
float GraphUtil::computeMinDiffAngle(std::vector<float> *data1, std::vector<float> *data2) {
	float ret = 0.0f;

	if (data1->size() <= data2->size()) {
		std::vector<bool> paired2;
		for (int i = 0; i < data2->size(); i++) {
			paired2.push_back(false);
		}

		for (int i = 0; i < data1->size(); i++) {
			float min_diff = std::numeric_limits<float>::max();


			int min_id = -1;
			for (int j = 0; j < data2->size(); j++) {
				if (paired2[j]) continue;

				float diff = diffAngle(data1->at(i), data2->at(j));
				if (diff < min_diff) {
					min_diff = diff;
					min_id = j;
				}
			}

			paired2[min_id] = true;
			ret += min_diff;
		}
	} else {
		std::vector<bool> paired1;
		for (int i = 0; i < data1->size(); i++) {
			paired1.push_back(false);
		}

		for (int i = 0; i < data2->size(); i++) {
			float min_diff = std::numeric_limits<float>::max();


			int min_id = -1;
			for (int j = 0; j < data1->size(); j++) {
				if (paired1[j]) continue;

				float diff = fabs(diffAngle(data2->at(i), data1->at(j)));
				if (diff < min_diff) {
					min_diff = diff;
					min_id = j;
				}
			}

			paired1[min_id] = true;
			ret += min_diff;
		}
	}

	return ret;
}

/**
 * 角度を正規化し、[-PI , PI]の範囲にする。
 */
float GraphUtil::normalizeAngle(float angle) {
	// まずは、正の値に変換する
	if (angle < 0.0f) {
		angle += ((int)(fabs(angle) / M_PI / 2.0f) + 1) * M_PI * 2;
	}

	// 次に、[0, PI * 2]の範囲にする
	angle -= (int)(angle / M_PI / 2.0f) * M_PI * 2;

	// 最後に、[-PI, PI]の範囲にする
	if (angle > M_PI) angle = M_PI * 2.0f - angle;

	return angle;
}

/**
 * Compute the difference in angle that is normalized in the range of [0, PI].
 */
float GraphUtil::diffAngle(const QVector2D& dir1, const QVector2D& dir2) {
	float ang1 = atan2f(dir1.y(), dir1.x());
	float ang2 = atan2f(dir2.y(), dir2.x());

	return fabs(normalizeAngle(ang1 - ang2));
}

/**
 * Compute the difference in angle that is normalized in the range of [0, PI].
 */
float GraphUtil::diffAngle(float angle1, float angle2) {
	return fabs(normalizeAngle(angle1 - angle2));
}

/**
 * 対応する頂点が与えられている時に、２つの道路網のトポロジーの違いを数値化して返却する。
 * トポロジーの違いなので、座標は一切関係ない。隣接ノードとの接続性のみを考慮する。
 *
 * @param w_connectivity		対応するエッジがない場合のペナルティ
 * @param w_split				対応が重複している場合のペナルティ
 * @param w_angle				エッジの角度のペナルティ
 * @param w_distance			対応する頂点の距離に対するペナルティ
 */
float GraphUtil::computeDissimilarity(RoadGraph* roads1, QMap<RoadVertexDesc, RoadVertexDesc>& map1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map2, float w_connectivity, float w_split, float w_angle, float w_distance) {
	float penalty = 0.0f;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// コネクティビティに基づいたペナルティの計上
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		if (!roads1->graph[*vi]->valid) continue;

		if (map1.contains(*vi)) {
			RoadVertexDesc v2 = map1[*vi];

			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads1->graph); ei != eend; ++ei) {
				if (!roads1->graph[*ei]->valid) continue;

				RoadVertexDesc v1b = boost::target(*ei, roads1->graph);
				RoadVertexDesc v2b = map1[v1b];

				if (v2 == v2b || !isConnected(roads2, v2, v2b)) { // 対応ノード間が接続されてない場合
				//if (v2 == v2b || !roads2->isConnected(v2, v2b)) { // キャッシュによる高速化（ただし、事前にconnectivityを計算する必要有り
					penalty += roads1->graph[*ei]->getLength() * roads1->graph[*ei]->weight * w_connectivity;
				} else {
					QVector2D dir1 = roads1->graph[v1b]->getPt() - roads1->graph[*vi]->getPt();
					QVector2D dir2 = roads2->graph[v2b]->getPt() - roads2->graph[v2]->getPt();
					if (dir1.lengthSquared() > 0.0f && dir2.lengthSquared() > 0.0f) {
						penalty += diffAngle(dir1, dir2) * w_angle;
					} else {
						// どちらかのエッジの長さ＝０、つまり、エッジがないので、コネクティビティのペナルティを課す
						// 道路網１の方のエッジの長さが０の場合、ペナルティは０となるが、
						// 道路網２の計算の際に、ペナルティが加算されるので、良いだろう。
						penalty += roads1->graph[*ei]->getLength() * roads1->graph[*ei]->weight * w_connectivity;
					}
				}
			}
		} else { // 当該ノードに対応するノードがない場合は、全てのエッジをペナルティとして計上する
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads1->graph); ei != eend; ++ei) {
				if (!roads1->graph[*ei]->valid) continue;

				RoadVertexDesc v1b = boost::target(*ei, roads1->graph);

				penalty += roads1->graph[*ei]->getLength() * roads1->graph[*ei]->weight * w_connectivity;
			}
		}
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		if (!roads2->graph[*vi]->valid) continue;

		if (map2.contains(*vi)) {
			RoadVertexDesc v1 = map2[*vi];

			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads2->graph); ei != eend; ++ei) {
				if (!roads2->graph[*ei]->valid) continue;

				RoadVertexDesc v2b = boost::target(*ei, roads2->graph);
				RoadVertexDesc v1b = map2[v2b];

				if (v1 == v1b || !isConnected(roads1, v1, v1b)) { // 対応ノード間が接続されてない場合
				//if (v1 == v1b || !roads1->isConnected(v1, v1b)) { // キャッシュによる高速化（ただし、事前にconnectivityを計算する必要有り
					penalty += roads2->graph[*ei]->getLength() * roads2->graph[*ei]->weight * w_connectivity;
				} else {
					QVector2D dir1 = roads1->graph[v1b]->getPt() - roads1->graph[v1]->getPt();
					QVector2D dir2 = roads2->graph[v2b]->getPt() - roads2->graph[*vi]->getPt();
					if (dir1.lengthSquared() > 0.0f && dir2.lengthSquared() > 0.0f) {
						penalty += diffAngle(dir1, dir2) * w_angle;
					} else {
						// どちらかのエッジの長さ＝０、つまり、エッジがないので、コネクティビティのペナルティを課す
						// 道路網２の方のエッジの長さが０の場合、ペナルティは０となるが、
						// 道路網１の計算の際に、ペナルティが加算されるので、良いだろう。
						penalty += roads2->graph[*ei]->getLength() * roads2->graph[*ei]->weight * w_connectivity;
					}
				}
			}
		} else { // 当該ノードに対応するノードがない場合は、全てのエッジをペナルティとして計上する
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(*vi, roads2->graph); ei != eend; ++ei) {
				if (!roads2->graph[*ei]->valid) continue;

				RoadVertexDesc v2b = boost::target(*ei, roads2->graph);

				penalty += roads2->graph[*ei]->getLength() * roads2->graph[*ei]->weight * w_connectivity;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 重複マッチング（モーフィングの際に、スプリットが発生）によるペナルティの計上
	QSet<RoadVertexDesc> used;
	for (QMap<RoadVertexDesc, RoadVertexDesc>::iterator it = map1.begin(); it != map1.end(); ++it) {
		if (used.contains(it.value())) {
			penalty += w_split;
		} else {
			used.insert(it.value());
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 頂点に距離に関するペナルティの計上
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		if (!roads1->graph[*vi]->valid) continue;

		if (map1.contains(*vi)) {
			RoadVertexDesc v2 = map1[*vi];

			penalty+= (roads1->graph[*vi]->pt - roads2->graph[v2]->pt).length() * w_distance;
		} else {
			// 対応する頂点がない場合、ペナルティはなし？
		}
	}

	return penalty;
}

/**
 * 対応する頂点が与えられている時に、２つの道路網のトポロジーの違いを数値化して返却する。
 * トポロジーの違いなので、座標は一切関係ない。隣接ノードとの接続性のみを考慮する。
 *
 * @param w_matching			対応するエッジがない場合のペナルティ
 */
float GraphUtil::computeDissimilarity2(RoadGraph* roads1, QMap<RoadVertexDesc, RoadVertexDesc>& map1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map2, float w_matching, float w_split, float w_angle, float w_distance) {
	float penalty = 0.0f;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 道路網１の各エッジについて、対応エッジがない場合のペナルティ
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads1->graph);
		
		if (!map1.contains(src) || !map1.contains(tgt)) {
			// 対応エッジがないので、当該エッジのImportanceに基づいて、ペナルティを追加
			penalty += roads1->graph[*ei]->importance * w_matching;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 道路網２の各エッジについて、対応エッジがない場合のペナルティ
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		if (!roads2->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads2->graph);
		
		if (!map2.contains(src) || !map2.contains(tgt)) {
			// 対応エッジがないので、当該エッジのImportanceに基づいて、ペナルティを追加
			penalty += roads2->graph[*ei]->importance * w_matching;
		}
	}

	return penalty;
}

/**
 * Return the similarity of two road graphs.
 */
float GraphUtil::computeSimilarity(RoadGraph* roads1, QMap<RoadVertexDesc, RoadVertexDesc>& map1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map2, float w_connectivity, float w_angle) {
	float score = 0.0f;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// For each edge of the 1st road graph, if there is a corresponding edge, increase the score.
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;

		RoadVertexDesc src1 = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt1 = boost::target(*ei, roads1->graph);
		
		if (map1.contains(src1) && map1.contains(tgt1)) {
			RoadVertexDesc src2 = map1[src1];
			RoadVertexDesc tgt2 = map1[tgt1];

			// increase the score
			//score += roads1->graph[*ei]->importance;
			score += w_connectivity;

			// increase the score according to the difference in the angle of the edges.
			float angle = diffAngle(roads1->graph[tgt1]->pt - roads1->graph[src1]->pt, roads2->graph[tgt2]->pt - roads2->graph[src2]->pt);
			score += (M_PI - angle) / M_PI * w_angle;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// For each edge of the 2nd road graph, if there is a corresponding edge, increase the score.
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		if (!roads2->graph[*ei]->valid) continue;

		RoadVertexDesc src2 = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt2 = boost::target(*ei, roads2->graph);
		
		if (map2.contains(src2) && map2.contains(tgt2)) {
			RoadVertexDesc src1 = map2[src2];
			RoadVertexDesc tgt1 = map2[tgt2];

			// increase the score
			//score += roads2->graph[*ei]->importance;
			score += w_connectivity;

			// increase the score according to the difference in the angle of the edges.
			float angle = diffAngle(roads1->graph[tgt1]->pt - roads1->graph[src1]->pt, roads2->graph[tgt2]->pt - roads2->graph[src2]->pt);
			score += (M_PI - angle) / M_PI * w_angle;
		}
	}

	return score;
}

/**
 * NearestNeighborに基づいて、２つの道路網のマッチングを行う。
 */
void GraphUtil::findCorrespondenceByNearestNeighbor(RoadGraph* roads1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map1, QMap<RoadVertexDesc, RoadVertexDesc>& map2) {
	if (getNumVertices(roads1) < getNumVertices(roads2)) {
		// 道路網１の各頂点に対して、対応する道路網２の頂点を探す
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
			if (!roads1->graph[*vi]->valid) continue;

			RoadVertexDesc v2 = getVertex(roads2, roads1->graph[*vi]->pt);
			map1[*vi] = v2;
			map2[v2] = *vi;
		}

		// 道路網２の各頂点に対して、まだペアがない場合は、対応する道路網１の頂点を探す
		for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
			if (!roads2->graph[*vi]->valid) continue;

			if (map2.contains(*vi)) continue;

			RoadVertexDesc v1 = getVertex(roads1, roads2->graph[*vi]->pt);
			map2[*vi] = v1;
		}
	} else {
		// 道路網２の各頂点に対して、対応する道路網１の頂点を探す
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
			if (!roads2->graph[*vi]->valid) continue;

			RoadVertexDesc v1 = getVertex(roads1, roads2->graph[*vi]->pt);
			map2[*vi] = v1;
			map1[v1] = *vi;
		}

		// 道路網１の各頂点に対して、まだペアがない場合は、対応する道路網２の頂点を探す
		for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
			if (!roads1->graph[*vi]->valid) continue;

			if (map1.contains(*vi)) continue;

			RoadVertexDesc v2 = getVertex(roads2, roads1->graph[*vi]->pt);
			map1[*vi] = v2;
		}
	}
}

/**
 * For two corresponding nodes, find the matching of outing edges.
 * Algorithm: minimize the maximum of the difference in angle of two corresponding edges.
 * If the degree is more than 6, the computation time will explode. Therefore, we should use an alternative approximation instead.
 */
QMap<RoadVertexDesc, RoadVertexDesc> GraphUtil::findCorrespondentEdges(RoadGraph* roads1, RoadVertexDesc parent1, std::vector<RoadVertexDesc> children1, RoadGraph* roads2, RoadVertexDesc parent2, std::vector<RoadVertexDesc> children2) {
	if (children1.size() > 6 || children2.size() > 6) {
		return findApproximateCorrespondentEdges(roads1, parent1, children1, roads2, parent2, children2);
	}

	QMap<RoadVertexDesc, RoadVertexDesc> map;

	std::vector<int> permutation;
	float min_diff = std::numeric_limits<float>::max();

	if (children1.size() <= children2.size()) {
		for (int i = 0; i < children2.size(); i++) {
			permutation.push_back(i);
		}

		for (int count = 0; count < 50000; count++) {
			// find the maximum of the difference in angle
			float diff = 0.0f;
			for (int i = 0; i < children1.size(); i++) {
				RoadEdgeDesc e1 = getEdge(roads1, parent1, children1[i]);
				RoadEdgeDesc e2 = getEdge(roads2, parent2, children2[permutation[i]]);

				QVector2D dir1;
				if ((roads1->graph[parent1]->pt - roads1->graph[e1]->polyLine[0]).length() < (roads1->graph[parent1]->pt - roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 1]).length()) {
					dir1 = roads1->graph[e1]->polyLine[1] - roads1->graph[e1]->polyLine[0];
				} else {
					dir1 = roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 2] - roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 1];
				}

				QVector2D dir2;
				if ((roads2->graph[parent2]->pt - roads2->graph[e2]->polyLine[0]).length() < (roads2->graph[parent2]->pt - roads1->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 1]).length()) {
					dir2 = roads2->graph[e2]->polyLine[1] - roads2->graph[e2]->polyLine[0];
				} else {
					dir2 = roads2->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 2] - roads2->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 1];
				}

				/*
				QVector2D dir1 = roads1->graph[children1[i]]->pt - roads1->graph[parent1]->pt;
				QVector2D dir2 = roads2->graph[children2[permutation[i]]]->pt - roads2->graph[parent2]->pt;
				*/

				diff = std::max(diff, diffAngle(dir1, dir2));
			}

			if (diff < min_diff) {
				min_diff = diff;
				map.clear();
				for (int i = 0; i < children1.size(); i++) {
					map[children1[i]] = children2[permutation[i]];
				}
			}

			if (!std::next_permutation(permutation.begin(), permutation.end())) break;
		}
	} else {
		for (int i = 0; i < children1.size(); i++) {
			permutation.push_back(i);
		}

		for (int count = 0; count < 50000; count++) {
			// find the maximum of difference in angle
			float diff = 0.0f;
			for (int i = 0; i < children2.size(); i++) {
				RoadEdgeDesc e1 = getEdge(roads1, parent1, children1[permutation[i]]);
				RoadEdgeDesc e2 = getEdge(roads2, parent2, children2[i]);

				QVector2D dir1;
				if ((roads1->graph[parent1]->pt - roads1->graph[e1]->polyLine[0]).length() < (roads1->graph[parent1]->pt - roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 1]).length()) {
					dir1 = roads1->graph[e1]->polyLine[1] - roads1->graph[e1]->polyLine[0];
				} else {
					dir1 = roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 2] - roads1->graph[e1]->polyLine[roads1->graph[e1]->polyLine.size() - 1];
				}

				QVector2D dir2;
				if ((roads2->graph[parent2]->pt - roads2->graph[e2]->polyLine[0]).length() < (roads2->graph[parent2]->pt - roads1->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 1]).length()) {
					dir2 = roads2->graph[e2]->polyLine[1] - roads2->graph[e2]->polyLine[0];
				} else {
					dir2 = roads2->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 2] - roads2->graph[e2]->polyLine[roads2->graph[e2]->polyLine.size() - 1];
				}

				//QVector2D dir1 = roads1->graph[children1[permutation[i]]]->pt - roads1->graph[parent1]->pt;
				//QVector2D dir2 = roads2->graph[children2[i]]->pt - roads2->graph[parent2]->pt;

				diff = std::max(diff, diffAngle(dir1, dir2));
			}

			if (diff < min_diff) {
				min_diff = diff;
				map.clear();
				for (int i = 0; i < children2.size(); i++) {
					map[children1[permutation[i]]] = children2[i];
				}
			}

			if (!std::next_permutation(permutation.begin(), permutation.end())) break;
		}
	}

	return map;
}

/**
 * For two corresponding nodes, find the matching of outing edges.
 * Algorithm: This is an approximation algorithm. Find the most similar pair of edges in terms of their angles, and make the a pair. Keep this process until there is no edge in one of the children lists.
 */
QMap<RoadVertexDesc, RoadVertexDesc> GraphUtil::findApproximateCorrespondentEdges(RoadGraph* roads1, RoadVertexDesc parent1, std::vector<RoadVertexDesc> children1, RoadGraph* roads2, RoadVertexDesc parent2, std::vector<RoadVertexDesc> children2) {
	QMap<RoadVertexDesc, RoadVertexDesc> map;
	QList<int> used1;
	QList<int> used2;

	while (true) {
		float min_diff = std::numeric_limits<float>::max();
		int min_i = -1;
		int min_j = -1;

		for (int i = 0; i < children1.size(); i++) {
			if (used1.contains(i)) continue;

			for (int j = 0; j < children2.size(); j++) {
				if (used2.contains(j)) continue;

				float diff = diffAngle(roads1->graph[children1[i]]->pt - roads1->graph[parent1]->pt, roads2->graph[children2[j]]->pt - roads2->graph[parent2]->pt);
				if (diff < min_diff) {
					min_diff = diff;
					min_i = i;
					min_j = j;
				}
			}
		}

		if (min_i == -1) break;

		map[children1[min_i]] = children2[min_j];
		used1.push_back(min_i);
		used2.push_back(min_j);
	}

	return map;
}

/**
 * Find the correspondence in two road graphs.
 */
void GraphUtil::findCorrespondence(RoadGraph* roads1, AbstractForest* forest1, RoadGraph* roads2, AbstractForest* forest2, bool findAllMatching, float threshold_angle, QMap<RoadVertexDesc, RoadVertexDesc>& map1, QMap<RoadVertexDesc, RoadVertexDesc>& map2) {
	std::list<RoadVertexDesc> seeds1;
	std::list<RoadVertexDesc> seeds2;

	// For each root edge
	for (int i = 0; i < forest1->getRoots().size(); i++) {
		RoadVertexDesc v1 = forest1->getRoots()[i];
		RoadVertexDesc v2 = forest2->getRoots()[i];

		// Match the root vertices
		map1[v1] = v2;
		map2[v2] = v1;

		// register the root vertices as seeds.
		seeds1.push_back(v1);
		seeds2.push_back(v2);
	}

	while (!seeds1.empty()) {
		RoadVertexDesc parent1 = seeds1.front();
		seeds1.pop_front();
		RoadVertexDesc parent2 = seeds2.front();
		seeds2.pop_front();

		// If there is no child, skip it.
		if (forest1->getChildren(parent1).size() == 0 && forest2->getChildren(parent2).size() == 0) continue;

		// retrieve the children list
		std::vector<RoadVertexDesc> children1 = forest1->getChildren(parent1);
		std::vector<RoadVertexDesc> children2 = forest2->getChildren(parent2);

		// retrieve the matching for the children lists.
		QMap<RoadVertexDesc, RoadVertexDesc> children_map = findCorrespondentEdges(roads1, parent1, children1, roads2, parent2, children2);
		for (QMap<RoadVertexDesc, RoadVertexDesc>::iterator it = children_map.begin(); it != children_map.end(); ++it) {
			RoadVertexDesc child1 = it.key();
			RoadVertexDesc child2 = it.value();

			// if the difference in angle is too large, skip this pair.
			if (diffAngle(roads1->graph[child1]->pt - roads1->graph[parent1]->pt, roads2->graph[child2]->pt - roads2->graph[parent2]->pt) > threshold_angle) continue;

			// update the matching
			map1[child1] = child2;
			map2[child2] = child1;

			// set fullyPaired flags
			roads1->graph[getEdge(roads1, parent1, child1)]->fullyPaired = true;
			roads2->graph[getEdge(roads2, parent2, child2)]->fullyPaired = true;

			seeds1.push_back(child1);
			seeds2.push_back(child2);
		}

		if (!findAllMatching) continue;

		// find the matching for the remained children
		while (true) {
			RoadVertexDesc child1, child2;
			if (!forceMatching(roads1, parent1, forest1, map1, roads2, parent2, forest2, map2, child1, child2)) break;

			// update the matching
			map1[child1] = child2;
			map2[child2] = child1;

			seeds1.push_back(child1);
			seeds2.push_back(child2);
		}
	}
}

/**
 * 相手のいない子ノードの中の１つに対して、対応する道路網の親ノードに無理やり対応させ、そのペアを返却する。
 * 相手のいない子ノードが１つもない場合は、falseを返却する。
 */
bool GraphUtil::forceMatching(RoadGraph* roads1, RoadVertexDesc parent1, AbstractForest* forest1, QMap<RoadVertexDesc, RoadVertexDesc>& map1, RoadGraph* roads2, RoadVertexDesc parent2, AbstractForest* forest2, QMap<RoadVertexDesc, RoadVertexDesc>& map2, RoadVertexDesc& child1, RoadVertexDesc& child2) {
	float min_angle = std::numeric_limits<float>::max();
	int min_id1;
	int min_id2;

	// retrieve the children list
	std::vector<RoadVertexDesc> children1 = forest1->getChildren(parent1);
	std::vector<RoadVertexDesc> children2 = forest2->getChildren(parent2);

	// ベストペアが見つからない、つまり、一方のリストが、全てペアになっている場合
	for (int i = 0; i < children1.size(); i++) {
		if (map1.contains(children1[i])) continue;
		if (!roads1->graph[children1[i]]->valid) continue;

		// 相手の親ノードをコピーしてマッチさせる
		RoadVertex* v = new RoadVertex(roads2->graph[parent2]->getPt());
		RoadVertexDesc v_desc = boost::add_vertex(roads2->graph);
		roads2->graph[v_desc] = v;

		RoadEdgeDesc e1_desc = GraphUtil::getEdge(roads1, parent1, children1[i]);

		// 相手の親ノードと子ノードの間にエッジを作成する
		//RoadEdgeDesc e2_desc = GraphUtil::addEdge(roads2, parent2, v_desc, roads1->graph[e1_desc]->lanes, roads1->graph[e1_desc]->type, roads1->graph[e1_desc]->oneWay);
		RoadEdgeDesc e2_desc = GraphUtil::addEdge(roads2, parent2, v_desc, roads1->graph[e1_desc]);
		roads2->graph[e2_desc]->polyLine.clear();
		roads2->graph[e2_desc]->addPoint(roads2->graph[parent2]->pt);
		roads2->graph[e2_desc]->addPoint(roads2->graph[v_desc]->pt);

		forest2->addChild(parent2, v_desc);

		child1 = children1[i];
		child2 = v_desc;

		return true;
	}

	for (int i = 0; i < children2.size(); i++) {
		if (map2.contains(children2[i])) continue;
		if (!roads2->graph[children2[i]]->valid) continue;

		// 相手の親ノードをコピーしてマッチさせる
		RoadVertex* v = new RoadVertex(roads1->graph[parent1]->getPt());
		RoadVertexDesc v_desc = boost::add_vertex(roads1->graph);
		roads1->graph[v_desc] = v;

		RoadEdgeDesc e2_desc = GraphUtil::getEdge(roads2, parent2, children2[i]);

		// 相手の親ノードと子ノードの間にエッジを作成する
		//GraphUtil::addEdge(roads1, parent1, v_desc, roads2->graph[e2_desc]->lanes, roads2->graph[e2_desc]->type, roads2->graph[e2_desc]->oneWay);
		RoadEdgeDesc e1_desc = GraphUtil::addEdge(roads1, parent1, v_desc, roads2->graph[e2_desc]);
		roads1->graph[e1_desc]->polyLine.clear();
		roads1->graph[e1_desc]->addPoint(roads1->graph[parent1]->pt);
		roads1->graph[e1_desc]->addPoint(roads1->graph[v_desc]->pt);

		forest1->addChild(parent1, v_desc);

		child1 = v_desc;
		child2 = children2[i];

		return true;
	}

	// No pair is found, i.e. all the children should have pairs.
	return false;
}

/**
 * 与えられた数列の、先頭の値を１インクリメントする。
 * N進法なので、Nになったら、桁が繰り上がる。つまり、次の要素の値を１インクリメントする。
 * ex. {1, 2, 3} => {2, 2, 3}
 * ex. {N-1, 3, 3} => {0, 4, 3}
 * ex. {N-1, N-1, 3} => {0, 0, 4}
 */
bool GraphUtil::nextSequence(std::vector<int>& seq, int N) {
	int index = 0;
	while (true) {
		if (seq[index] < N - 1) break;

		seq[index] = 0;

		if (++index >= seq.size()) break;
	}

	if (index < seq.size()) {
		seq[index]++;
		return true;
	} else {
		return false;
	}
}

/**
 * Return the average edge length.
 */
float GraphUtil::computeAvgEdgeLength(RoadGraph* roads) {
	float length = 0.0f;
	int count = 0;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		length += (roads->graph[src]->pt - roads->graph[tgt]->pt).length();
		count++;
	}

	if (count == 0) return 0;
	else return length / (float)count;
}

/**
 * ２つの道路網について、似ているエッジペアを類似度でソートして、トップNを返却する。
 */
QList<EdgePair> GraphUtil::getClosestEdgePairs(RoadGraph* roads1, RoadGraph* roads2, int num) {
	QList<EdgePair> pairs;

	// Importance順に並べたエッジリストを取得
	QList<RoadEdgeDesc> edges1 = roads1->getOrderedEdgesByImportance();
	QList<RoadEdgeDesc> edges2 = roads2->getOrderedEdgesByImportance();

	while (pairs.size() < num && !edges1.empty() && !edges2.empty()) {
		float min_diff = std::numeric_limits<float>::max();
		RoadEdgeDesc e1;
		RoadEdgeDesc e2;

		// 道路網１のTop10 Importantエッジに対して
		int topN = std::min(10, edges1.size());
		for (int i = 0; i < topN; i++) {
			// 似ている対応エッジを道路網２から探す
			for (int j = 0; j < edges2.size(); j++) {
				float diff = GraphUtil::computeDissimilarityOfEdges(roads1, edges1[i], roads2, edges2[j]);
				if (diff < min_diff) {
					min_diff = diff;
					e1 = edges1[i];
					e2 = edges2[j];
				}
			}
		}

		// 道路網２のTop10 Importantエッジに対して
		topN = std::min(10, edges2.size());
		for (int i = 0; i < topN; i++) {
			// 似ている対応エッジを道路網１ら探す
			for (int j = 0; j < edges1.size(); j++) {
				float diff = GraphUtil::computeDissimilarityOfEdges(roads1, edges1[j], roads2, edges2[i]);
				if (diff < min_diff) {
					min_diff = diff;
					e1 = edges1[j];
					e2 = edges2[i];
				}
			}
		}

		// 抽出された最も似ているエッジペアを、リストに登録する
		pairs.push_back(EdgePair(e1, e2));

		// 選択されたペアから、両端のノードを取得
		RoadVertexDesc src1 = boost::source(e1, roads1->graph);
		RoadVertexDesc tgt1 = boost::target(e1, roads1->graph);
		RoadVertexDesc src2 = boost::source(e2, roads2->graph);
		RoadVertexDesc tgt2 = boost::target(e2, roads2->graph);

		//エッジリストから、使用したエッジに含まれる頂点を含むエッジを削除
		for (int j = edges1.size() - 1; j >= 0; j--) {
			if (boost::source(edges1[j], roads1->graph) == src1 || boost::source(edges1[j], roads1->graph) == tgt1 || boost::target(edges1[j], roads1->graph) == src1 || boost::target(edges1[j], roads1->graph) == tgt1) {
				edges1.removeAt(j);
			}
		}
		for (int j = edges2.size() - 1; j >= 0; j--) {
			if (boost::source(edges2[j], roads2->graph) == src2 || boost::source(edges2[j], roads2->graph) == tgt2 || boost::target(edges2[j], roads2->graph) == src2 || boost::target(edges2[j], roads2->graph) == tgt2) {
				edges2.removeAt(j);
			}
		}
	}

	return pairs;
}

/**
 * Apply the global rigid ICP in order to fit the 1st road graph to the 2nd road graph in the least square manner.
 */
void GraphUtil::rigidICP(RoadGraph* roads1, RoadGraph* roads2, QList<EdgePair>& pairs) {
	cv::Mat src(pairs.size() * 2, 2, CV_32FC2);
	cv::Mat dst(pairs.size() * 2, 2, CV_32FC2);

	for (int i = 0; i < pairs.size(); i++) {
		RoadEdgeDesc e1 = pairs[i].edge1;
		RoadEdgeDesc e2 = pairs[i].edge2;

		// エッジの両端頂点を取得
		RoadVertexDesc src1 = boost::source(e1, roads1->graph);
		RoadVertexDesc tgt1 = boost::target(e1, roads1->graph);
		RoadVertexDesc src2 = boost::source(e2, roads2->graph);
		RoadVertexDesc tgt2 = boost::target(e2, roads2->graph);

		// もしsrc1-tgt2、tgt1-src2の方が近かったら、src2とtgt2を入れ替える
		if ((roads1->graph[src1]->pt - roads2->graph[src2]->pt).length() + (roads1->graph[tgt1]->pt - roads2->graph[tgt2]->pt).length() > (roads1->graph[src1]->pt - roads2->graph[tgt2]->pt).length() + (roads1->graph[tgt1]->pt - roads2->graph[src2]->pt).length()) {
			src2 = boost::target(e2, roads2->graph);
			tgt2 = boost::source(e2, roads2->graph);
		}

		// 頂点の座標を行列に格納
		src.at<float>(i * 2, 0) = roads1->graph[src1]->pt.x();
		src.at<float>(i * 2, 1) = roads1->graph[src1]->pt.y();
		src.at<float>(i * 2 + 1, 0) = roads1->graph[tgt1]->pt.x();
		src.at<float>(i * 2 + 1, 1) = roads1->graph[tgt1]->pt.y();
		dst.at<float>(i * 2, 0) = roads2->graph[src2]->pt.x();
		dst.at<float>(i * 2, 1) = roads2->graph[src2]->pt.y();
		dst.at<float>(i * 2 + 1, 0) = roads2->graph[tgt2]->pt.x();
		dst.at<float>(i * 2 + 1, 1) = roads2->graph[tgt2]->pt.y();
	}

	// Rigid ICP 変換行列を計算
	cv::Mat transformMat = cv::estimateRigidTransform(src, dst, false);

	// 道路網１の頂点座標を、変換行列を使って更新
	src = convertVerticesToCVMatrix(roads1, false);
	cv::Mat src2;
	cv::transform(src, src2, transformMat);

	// 道路網１の頂点座標を実際に更新する
	RoadVertexIter vi, vend;
	int count = 0;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		roads1->graph[*vi]->pt.setX(src2.at<float>(count, 0));
		roads1->graph[*vi]->pt.setY(src2.at<float>(count, 1));
		count++;
	}

	// 道路網１のエッジの座標も更新する
	src = convertEdgesToCVMatrix(roads1, false);
	cv::transform(src, src2, transformMat);

	// 道路網１のエッジ座標を実際に更新する
	RoadEdgeIter ei, eend;
	count = 0;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		for (int i = 0; i < roads1->graph[*ei]->polyLine.size(); i++) {
			roads1->graph[*ei]->polyLine[i].setX(src2.at<float>(count, 0));
			roads1->graph[*ei]->polyLine[i].setY(src2.at<float>(count, 1));
			count++;
		}
	}
}

/**
 * 道路網の頂点座標を、Nx2の行列に変換する
 */
cv::Mat GraphUtil::convertVerticesToCVMatrix(RoadGraph* roads, bool onlyValidVertex) {
	cv::Mat ret(getNumVertices(roads, onlyValidVertex), 2, CV_32FC2);

	RoadVertexIter vi, vend;
	int count = 0;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (onlyValidVertex && !roads->graph[*vi]->valid) continue;

		ret.at<float>(count, 0) = roads->graph[*vi]->pt.x();
		ret.at<float>(count, 1) = roads->graph[*vi]->pt.y();
		count++;
	}

	return ret;
}

/**
 * 道路網のエッジ座標を、Nx2の行列に変換する
 */
cv::Mat GraphUtil::convertEdgesToCVMatrix(RoadGraph* roads, bool onlyValidVertex) {
	std::vector<QVector2D> data;
	
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (onlyValidVertex && !roads->graph[*ei]->valid) continue;

		for (int i = 0; i < roads->graph[*ei]->polyLine.size(); i++) {
			data.push_back(roads->graph[*ei]->polyLine[i]);
		}
	}
	
	cv::Mat ret(data.size(), 2, CV_32FC2);

	for (int i = 0; i < data.size(); i++) {
		ret.at<float>(i, 0) = data[i].x();
		ret.at<float>(i, 1) = data[i].y();
	}

	return ret;
}

/**
 * グリッドスタイルの道路網を作成する。
 *
 * @param size		一辺の長さ [m]
 * @param num		一辺のノード数
 */
RoadGraph* GraphUtil::createGridNetwork(float size, int num) {
	RoadGraph* roads = new RoadGraph();

	// 各エッジの長さ
	float length = size / (float)(num - 1);

	// 原点座標
	QVector2D orig(-size / 2.0f, -size / 2.0f);

	// ノードを作成
	for (int i = 0; i < num - 2; i++) {
		for (int j = 0; j < num; j++) {
			RoadVertex* v = new RoadVertex(orig + QVector2D(j * length, i * length + length));
			RoadVertexDesc desc = boost::add_vertex(roads->graph);
			roads->graph[desc] = v;
		}
	}
	for (int i = 0; i < num - 2; i++) {
		RoadVertex* v = new RoadVertex(orig + QVector2D(i * length + length, 0));
		RoadVertexDesc desc = boost::add_vertex(roads->graph);
		roads->graph[desc] = v;
	}
	for (int i = 0; i < num - 2; i++) {
		RoadVertex* v = new RoadVertex(orig + QVector2D(i * length + length, size));
		RoadVertexDesc desc = boost::add_vertex(roads->graph);
		roads->graph[desc] = v;
	}

	// エッジを作成
	for (int i = 0; i < num - 2; i++) {
		for (int j = 0; j < num - 1; j++) {
			addEdge(roads, i * num + j, i * num + j + 1, 2, 2);
		}
	}
	for (int i = 0; i < num - 3; i++) {
		for (int j = 0; j < num - 2; j++) {
			addEdge(roads, i * num + 1 + j, i * num + 1 + j + num, 2, 2);
		}
	}
	for (int i = 0; i < num - 2; i++) {
		addEdge(roads, num * (num - 2) + i, i + 1, 2, 2);
		addEdge(roads, num * (num - 2) + (num - 2) + i, num * (num - 3) + i + 1, 2, 2);
	}

	return roads;
}

/**
 * 曲がったスタイルの道路網を作成する。
 *
 * @param size		一辺の長さ [m]
 * @param num		一辺のノード数
 * @param angle		傾ける角度 [rad]
 */
RoadGraph* GraphUtil::createCurvyNetwork(float size, int num, float angle) {
	RoadGraph* roads = new RoadGraph();

	// 各エッジの長さ
	float length = size / (float)(num - 1);

	// 原点座標
	QVector2D orig(-size / 2.0f, -size / 2.0f);

	// ノードを作成
	for (int i = 0; i < num - 2; i++) {
		for (int j = 0; j < num; j++) {
			QVector2D pos = orig + QVector2D(j * length, i * length + length);
			QVector2D pos2;
			pos2.setX(pos.x() * cosf(angle) - pos.y() * sinf(angle));
			pos2.setY(pos.x() * sinf(angle) + pos.y() * cosf(angle));
			RoadVertex* v = new RoadVertex(pos2);
			RoadVertexDesc desc = boost::add_vertex(roads->graph);
			roads->graph[desc] = v;
		}
	}
	for (int i = 0; i < num - 2; i++) {
		QVector2D pos = orig + QVector2D(i * length + length, 0);
		QVector2D pos2;
		pos2.setX(pos.x() * cosf(angle) - pos.y() * sinf(angle));
		pos2.setY(pos.x() * sinf(angle) + pos.y() * cosf(angle));
		RoadVertex* v = new RoadVertex(pos2);
		RoadVertexDesc desc = boost::add_vertex(roads->graph);
		roads->graph[desc] = v;
	}
	for (int i = 0; i < num - 2; i++) {
		QVector2D pos = orig + QVector2D(i * length + length, size);
		QVector2D pos2;
		pos2.setX(pos.x() * cosf(angle) - pos.y() * sinf(angle));
		pos2.setY(pos.x() * sinf(angle) + pos.y() * cosf(angle));
		RoadVertex* v = new RoadVertex(pos2);
		RoadVertexDesc desc = boost::add_vertex(roads->graph);
		roads->graph[desc] = v;
	}

	// エッジを作成
	for (int i = 0; i < num - 2; i++) {
		for (int j = 0; j < num - 1; j++) {
			RoadEdge* e = new RoadEdge(2, 2, false);
			QVector2D pos = orig + QVector2D(j * length, i * length + length);
			for (int k = 0; k <= 10; k++) {
				QVector2D pos2 = pos + QVector2D((float)k * 0.1f * length, length * 0.1f * sinf((float)k * M_PI * 2 * 0.1f));
				QVector2D pos3;
				pos3.setX(pos2.x() * cosf(angle) - pos2.y() * sinf(angle));
				pos3.setY(pos2.x() * sinf(angle) + pos2.y() * cosf(angle));
				e->addPoint(pos3);
			}
			
			std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(i * num + j, i * num + j + 1, roads->graph);
			roads->graph[edge_pair.first] = e;
		}
	}
	for (int i = 0; i < num - 3; i++) {
		for (int j = 0; j < num - 2; j++) {
			RoadEdge* e = new RoadEdge(2, 2, false);
			QVector2D pos = orig + QVector2D(j * length + length, i * length + length);
			for (int k = 0; k <= 10; k++) {
				QVector2D pos2 = pos + QVector2D(length * 0.1f * sinf((float)k * M_PI * 2 * 0.1f), (float)k * 0.1f * length);
				QVector2D pos3;
				pos3.setX(pos2.x() * cosf(angle) - pos2.y() * sinf(angle));
				pos3.setY(pos2.x() * sinf(angle) + pos2.y() * cosf(angle));
				e->addPoint(pos3);
			}
			
			std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(i * num + 1 + j, i * num + 1 + j + num, roads->graph);
			roads->graph[edge_pair.first] = e;

			//addEdge(roads, i * num + 1 + j, i * num + 1 + j + num, 2, 2);
		}
	}
	for (int i = 0; i < num - 2; i++) {
		RoadEdge* e = new RoadEdge(2, 2, false);
		QVector2D pos = orig + QVector2D(i * length + length, 0);
		for (int k = 0; k <= 10; k++) {
			QVector2D pos2 = pos + QVector2D(length * 0.1f * sinf((float)k * M_PI * 2 * 0.1f), (float)k * 0.1f * length);
			QVector2D pos3;
			pos3.setX(pos2.x() * cosf(angle) - pos2.y() * sinf(angle));
			pos3.setY(pos2.x() * sinf(angle) + pos2.y() * cosf(angle));
			e->addPoint(pos3);
		}
			
		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(num * (num - 2) + i, i + 1, roads->graph);
		roads->graph[edge_pair.first] = e;

		//addEdge(roads, num * (num - 2) + i, i + 1, 2, 2);
	}
	for (int i = 0; i < num - 2; i++) {
		RoadEdge* e = new RoadEdge(2, 2, false);
		QVector2D pos = orig + QVector2D(i * length + length, size - length);
		for (int k = 0; k <= 10; k++) {
			QVector2D pos2 = pos + QVector2D(length * 0.1f * sinf((float)k * M_PI * 2 * 0.1f), (float)k * 0.1f * length);
			QVector2D pos3;
			pos3.setX(pos2.x() * cosf(angle) - pos2.y() * sinf(angle));
			pos3.setY(pos2.x() * sinf(angle) + pos2.y() * cosf(angle));
			e->addPoint(pos3);
		}
			
		std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(num * (num - 2) + (num - 2) + i, num * (num - 3) + i + 1, roads->graph);
		roads->graph[edge_pair.first] = e;
		
		//addEdge(roads, num * (num - 2) + (num - 2) + i, num * (num - 3) + i + 1, 2, 2);
	}

	return roads;
}

/**
 * 放射線状の道路網を作成する。
 *
 * @param size		一辺の長さ [m]
 * @param num		円を何個作るか？
 * @param degree	中心から出るエッジの数
 */
RoadGraph* GraphUtil::createRadialNetwork(float size, int num, int degree) {
	RoadGraph* roads = new RoadGraph();

	float length = size / (float)(num + 1) / 2.0f;

	// 頂点を追加
	RoadVertex* v = new RoadVertex(QVector2D(0, 0));
	RoadVertexDesc desc = boost::add_vertex(roads->graph);
	roads->graph[desc] = v;

	for (int i = 0; i < num + 1; i++) {
		for (int j = 0; j < degree; j++) {
			float theta = (float)j / (double)degree * M_PI * 2.0f;
			RoadVertex* v = new RoadVertex(length * QVector2D((float)(i + 1) * cosf(theta), (float)(i + 1) * sinf(theta)));
			RoadVertexDesc desc = boost::add_vertex(roads->graph);
			roads->graph[desc] = v;
		}
	}

	// エッジを追加
	for (int i = 0; i < degree; i++) {
		addEdge(roads, 0, i + 1, 2, 2);
	}
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < degree; j++) {
			addEdge(roads, 1 + i * degree + j, 1 + (i + 1) * degree + j, 2, 2);
		}
	}
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < degree; j++) {
			float theta = (float)j / (double)degree * M_PI * 2.0f;
			float dt = 1.0f / (double)degree * M_PI * 2.0f;
			RoadEdge* e = new RoadEdge(2, 2, false);
			for (int k = 0; k <= 4; k++) {
				QVector2D pos = length * QVector2D((float)(i + 1) * cosf(theta + dt * (float)k / 4.0f), (float)(i + 1) * sinf(theta + dt * (float)k / 4.0f));
				e->addPoint(pos);
			}
			
			std::pair<RoadEdgeDesc, bool> edge_pair = boost::add_edge(1 + i * degree + j, 1 + i * degree + (j + 1) % degree, roads->graph);
			roads->graph[edge_pair.first] = e;
			
			//addEdge(roads, 1 + i * 12 + j, 1 + i * 12 + (j + 1) % 12, 2, 2);
		}
	}

	return roads;
}

/**
 * Print the statistics of the road graph.
 * Print the histogram of degrees and the historgram of lanes.
 */
void GraphUtil::printStatistics(RoadGraph* roads) {
	int degreesHistogram[10];
	int lanesHistogram[10];

	for (int i = 0; i < 10; i++) {
		degreesHistogram[i] = 0;
		lanesHistogram[i] = 0;
	}

	// degreeのヒストグラムを作成
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		int degree = getDegree(roads, *vi);
		if (degree < 10) {
			degreesHistogram[degree]++;
		}
	}

	qDebug() << "Degrees:";
	for (int i = 0; i < 10; i++) {
		qDebug() << i << ": " << degreesHistogram[i];
	}

	// レーン数のヒストグラムを作成
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		int lanes = roads->graph[*ei]->lanes;
		if (lanes < 10) {
			lanesHistogram[lanes]++;
		}
	}

	qDebug() << "Lanes:";
	for (int i = 0; i < 10; i++) {
		qDebug() << i << ": " << lanesHistogram[i];
	}
}
