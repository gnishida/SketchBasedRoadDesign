#pragma once

#include "Camera.h"
#include "RoadGraph.h"
#include "Sketch.h"
#include "RoadGraphRenderer.h"
#include <QGLWidget>
#include <qstring.h>
#include <qvector3d.h>

class MyMainWindow;

class GLWidget : public QGLWidget {
public:
	static float MIN_Z;
	static float MAX_Z;

public:
	MyMainWindow* mainWin;
	Camera* camera;
	RoadGraph* roads;
	Sketch* sketch;
	RoadGraph* ref_roads;
	RoadGraphRenderer* renderer;
	QPoint lastPos;

public:
	GLWidget(MyMainWindow* mainWin);
	~GLWidget();

	void drawScene();
	void newRoad();
	void openRoad(QString filename);
	void makeRoadsFromSketch();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	void mouseTo2D(int x, int y, QVector2D *result);
};

