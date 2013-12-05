#include "Canvas.h"
#include <qdockwidget.h>
#include <qpainter.h>
#include <qdebug.h>

Canvas::Canvas(QWidget *parent) : QGraphicsScene(parent) {
	currentLine = NULL;

	Line* line = new Line();
	line->points.push_back(QPoint(0, 0));
	line->points.push_back(QPoint(100, 100));
	addItem(line);
}

Canvas::~Canvas() {
}

void Canvas::mousePressEvent(QMouseEvent *event) {
	if (event->buttons() == Qt::LeftButton) {
		prevMousePos = event->pos();
		currentLine = new Line();
	} else if (event->buttons() == Qt::RightButton) {
	}
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
	if (event->buttons() == Qt::LeftButton) {
		currentLine->points.push_back(event->pos());

		prevMousePos = event->pos();
	}

	update();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
	if (event->buttons() == Qt::LeftButton) {
		addItem(currentLine);
	}

	update();
}
