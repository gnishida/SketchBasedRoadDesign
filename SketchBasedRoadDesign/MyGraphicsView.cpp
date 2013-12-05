#include "MyGraphicsView.h"
#include "Util.h"
#include <qvector2d.h>

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

/**
 * 新規追加したエッジによって、新たにできた交点をチェックし、交点でエッジを分割する。
 * さらに、近い交点同士を結合し、簡易化する。
 */
void MyGraphicsView::simplify(Line* newLine) {
	for (int i = 0; i < scene->items().size(); i++) {
		if (scene->items()[i] == newLine) continue;


		Line* line = (Line*)scene->items()[i];
		for (int j = 0; j < line->points.size() - 1; j++) {
			Line* newLine2 = new Line();
			*newLine2 = *newLine;

			for (int k = 0; k < newLine->points.size() - 1; k++) {
				float tab, tcd;
				QVector2D intPt;
				if (Util::segmentSegmentIntersectXY(line->points[j], line->points[j + 1], newLine->points[k], newLine->points[k + 1], &tab, &tcd, true, intPt)) {
					newLine2->points.push_back(newLine->points[k]);
					newLine2->points.push_back(intPt);
				} else {
					newLine2->points.push_back(newLine->points[k]);
				}
			}

			*newLine = *newLine2;
		}

	}
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

		simplify(currentLine);
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
