#include "BFSTree.h"
#include "GraphUtil.h"

/**
 * Constructor
 * root is the root node to be used to create the tree.
 */
BFSTree::BFSTree(RoadGraph* roads, RoadVertexDesc root) : AbstractForest(roads) {
	this->roots.push_back(root);

	buildForest();
}

BFSTree::~BFSTree() {
}

void BFSTree::buildForest() {
	QList<RoadVertexDesc> seeds;

	QMap<RoadEdgeDesc, bool> visitedEdge;
	QMap<RoadVertexDesc, bool> visitedVertex;

	RoadVertexDesc root = roots[0];

	// シードを登録する
	seeds.push_back(root);

	// ルート頂点を訪問済みとマークする
	visitedVertex[root] = true;

	// ルート頂点リストからスタートして、BFSで全頂点を訪問する
	while (!seeds.empty()) {
		RoadVertexDesc parent = seeds.front();
		seeds.pop_front();

		std::vector<RoadVertexDesc> children;

		// 隣接ノードリストを先に洗い出す
		std::vector<RoadVertexDesc> nodes;
		std::vector<RoadEdgeDesc> edges;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(parent, roads->graph); ei != eend; ++ei) {
			if (!roads->graph[*ei]->valid) continue;
			if (visitedEdge[*ei]) continue;

			// 隣接ノードを取得
			RoadVertexDesc child = boost::target(*ei, roads->graph);
			if (!roads->graph[child]->valid) continue;

			if (getParent(parent).contains(child)) continue;

			nodes.push_back(child);
			edges.push_back(*ei);

			// 当該エッジを通過したとマークする
			visitedEdge[*ei] = true;
		}

		// 洗い出した隣接ノードに対して、訪問する
		for (int i = 0; i < nodes.size(); i++) {
			RoadVertexDesc child = nodes[i];

			if (visitedVertex.contains(child)) { // 訪問済みの場合
				/*
				RoadEdgeDesc orig_e_desc = GraphUtil::getEdge(roads, parent, child);

				// もともとのエッジを無効にする
				roads->graph[orig_e_desc]->valid = false;

				// 対象ノードが訪問済みの場合、対象ノードをコピーして子ノードにする
				RoadVertexDesc child2 = GraphUtil::addVertex(roads, roads->graph[child]);
				roads->graph[child2]->virt = false;

				// エッジ作成
				RoadEdgeDesc e_desc = GraphUtil::addEdge(roads, parent, child2, roads->graph[orig_e_desc]);

				children.push_back(child2);
				*/

				children.push_back(child);
			} else { // 未訪問の場合
				visitedVertex[child] = true;

				children.push_back(child);

				seeds.push_back(child);
			}
		}

		this->children.insert(parent, children);
	}
}