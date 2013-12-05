#pragma once

#include <vector>
#include <qvector2d.h>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

typedef boost::geometry::model::d2::point_xy<double> point;

class Line : public QGraphicsItem {
public:
	//std::vector<QPointF> points;
	//boost::geometry::model::linestring<point> points;
	std::vector<QVector2D> points;

public:
	Line();
	~Line();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void simplify();
};

