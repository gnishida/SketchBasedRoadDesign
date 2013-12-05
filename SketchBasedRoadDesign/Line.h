#pragma once

#include <vector>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

typedef boost::geometry::model::d2::point_xy<double> point;

class Line : public QGraphicsItem {
public:
	//std::vector<QPointF> points;
	boost::geometry::model::linestring<point> points;

public:
	Line();
	~Line();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void simplify();
};

