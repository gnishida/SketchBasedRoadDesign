#pragma once

#include "RoadGraph.h"
#include "Renderable.h"

class RoadStyle;

class RoadGraphRenderer {
public:
	unsigned int dispList;

public:
	RoadGraphRenderer();

	void render(std::vector<Renderable>& renderables);
};

