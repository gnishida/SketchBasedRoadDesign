#include "RoadGraphRenderer.h"
#include "MyMainWindow.h"
#include <QtOpenGL>

RoadGraphRenderer::RoadGraphRenderer() {
}

void RoadGraphRenderer::render(std::vector<Renderable>& renderables) {
	for (int i = 0; i < renderables.size(); i++) {
		if (renderables[i].type == GL_LINES || renderables[i].type == GL_LINE_STRIP) {
			glLineWidth(renderables[i].size);
		}

		if (renderables[i].type == GL_POINTS) {
			glPointSize(renderables[i].size);
		}

		glBegin(renderables[i].type);
		for (int j = 0; j < renderables[i].vertices.size(); ++j) {
			glColor4f(renderables[i].vertices[j].color[0], renderables[i].vertices[j].color[1], renderables[i].vertices[j].color[2], renderables[i].vertices[j].color[3]);
			glNormal3f(renderables[i].vertices[j].normal[0], renderables[i].vertices[j].normal[1], renderables[i].vertices[j].normal[2]);
			glVertex3f(renderables[i].vertices[j].location[0], renderables[i].vertices[j].location[1], renderables[i].vertices[j].location[2]);
		}
		glEnd();
	}
}
