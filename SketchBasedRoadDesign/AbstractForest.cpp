#include "AbstractForest.h"

AbstractForest::AbstractForest(RoadGraph* roads) {
	this->roads = roads;
}

AbstractForest::~AbstractForest() {
}

/**
 * 子ノードのリストを返却する。
 */
std::vector<RoadVertexDesc>& AbstractForest::getChildren(RoadVertexDesc node) {
	if (!children.contains(node)) {
		std::vector<RoadVertexDesc> c;
		children[node] = c;
	}

	return children[node];
}

/**
 * 子ノードを追加する。
 */
void AbstractForest::addChild(RoadVertexDesc parent, RoadVertexDesc child) {
	std::vector<RoadVertexDesc> list = getChildren(parent);
	list.push_back(child);
	children[parent] = list;
}

/**
 * 親ノードのリストを返却する。
 */
QList<RoadVertexDesc> AbstractForest::getParent(RoadVertexDesc node) {
	QList<RoadVertexDesc> ret;

	for (QMap<RoadVertexDesc, std::vector<RoadVertexDesc> >::iterator it = children.begin(); it != children.end(); ++it) {
		RoadVertexDesc parent = it.key();
		for (int i = 0; i < children[parent].size(); i++) {
			if (children[parent][i] == node) {
				ret.push_back(parent);
			}
		}
	}

	return ret;
}

/**
 * ルートノードを返却する。
 */
QList<RoadVertexDesc> AbstractForest::getRoots() {
	return roots;
}
