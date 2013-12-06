#pragma once

#include "AbstractForest.h"
#include "RoadGraph.h"
#include <vector>

class BFSTree : public AbstractForest {
public:
	BFSTree(RoadGraph* roads, RoadVertexDesc root);
	~BFSTree();
	
	void buildForest();
};

