#pragma once

#include "Line.h"
#include <qgraphicsscene.h>
//#include <qlabel.h>
#include <QMouseEvent>

class Canvas : public QGraphicsScene {
private:
	QPoint prevMousePos;
	std::vector<Line> lines;
	Line* currentLine;

public:
	Canvas(QWidget *parent);
	~Canvas();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

public:
};

