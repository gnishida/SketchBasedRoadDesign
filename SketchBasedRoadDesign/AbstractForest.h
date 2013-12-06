#pragma once

#include "RoadGraph.h"
#include <qmap.h>
#include <vector>

class AbstractForest {
public:
	RoadGraph* roads;
	QMap<RoadVertexDesc, std::vector<RoadVertexDesc> > children;
	QList<RoadVertexDesc> roots;

public:
	AbstractForest(RoadGraph* roads);
	~AbstractForest();

	std::vector<RoadVertexDesc>& getChildren(RoadVertexDesc node);
	void addChild(RoadVertexDesc parent, RoadVertexDesc child);
	QList<RoadVertexDesc> getParent(RoadVertexDesc node);
	QList<RoadVertexDesc> getRoots();

	virtual void buildForest() = 0;
};

