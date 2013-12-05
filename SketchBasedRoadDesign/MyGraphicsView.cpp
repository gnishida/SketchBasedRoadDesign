#include "MyGraphicsView.h"


MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 800, 600);
	this->setScene(scene);

	/*
	pathItem = scene->addPath(QPainterPath());
	pathItem->setBrush(QColor(0, 0, 0));
	pathItem->setPen(Qt::NoPen);
	*/

	setRenderHint( QPainter::Antialiasing );
}

MyGraphicsView::~MyGraphicsView() {
}

void MyGraphicsView::mousePressEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		currentLine = new Line();
		scene->addItem(currentLine);

		QPointF pt = mapToScene(e->pos());

		currentLine->points.push_back(point(pt.x(), pt.y()));
	} else if (e->buttons() == Qt::RightButton) {
	}

	scene->update();
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent* e) {
	if (currentLine != NULL) {
		currentLine->simplify();
	}

	scene->update();
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent* e) {
	if (e->buttons() == Qt::LeftButton) {
		QPointF pt = mapToScene(e->pos());
		currentLine->points.push_back(point(pt.x(), pt.y()));
	}

	scene->update();
}
