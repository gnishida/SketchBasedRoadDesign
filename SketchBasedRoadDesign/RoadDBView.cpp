#include "RoadDBView.h"

RoadDBView::RoadDBView(QWidget *parent) : QGraphicsView(parent) {
	scene = new QGraphicsScene();
	this->setSceneRect(0, 0, 400, 300);

	this->setScene(scene);

	this->setBackgroundBrush(Qt::red);

	setRenderHint( QPainter::Antialiasing );
}

RoadDBView::~RoadDBView() {
}
