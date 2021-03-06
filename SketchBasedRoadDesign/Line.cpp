﻿#include "Line.h"

Line::Line() {
	this->setFlag(QGraphicsItem::ItemIsMovable);
}

Line::~Line() {
}

Line& Line::operator=(const Line& ref) {
	points = ref.points;
	return *this;
}

QRectF Line::boundingRect() const {
	QRectF rect(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (int i = 0; i < points.size(); i++) {
		if (points[i].x() < rect.left()) {
			rect.setLeft(points[i].x());
		}
		if (points[i].x() > rect.right()) {
			rect.setRight(points[i].x());
		}
		if (points[i].y() < rect.top()) {
			rect.setTop(points[i].y());
		}
		if (points[i].y() > rect.bottom()) {
			rect.setBottom(points[i].y());
		}
	}

	return rect;
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	for (int i = 0; i < points.size() - 1; i++) {
		painter->drawLine(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y());
	}

	for (int i = 0; i < points.size(); i++) {
		painter->drawRect(points[i].x() - 2, points[i].y() - 2, 5, 5);
	}
}

void Line::simplify() {
	boost::geometry::model::linestring<point> points2;
	for (int i = 0; i < points.size(); i++) {
		points2.push_back(point(points[i].x(), points[i].y()));
	}

    // Simplify it, using distance of 0.5 units
    boost::geometry::model::linestring<point> simplified;
    boost::geometry::simplify(points2, simplified, 2);

	points.clear();
	for (int i = 0; i < simplified.size(); i++) {
		points.push_back(QVector2D(simplified[i].x(), simplified[i].y()));
	}
}
