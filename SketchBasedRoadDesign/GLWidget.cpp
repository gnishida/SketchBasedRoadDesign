#include "GLWidget.h"
#include "MyMainWindow.h"
#include "GraphUtil.h"
#include <gl/GLU.h>
#include <qvector3d.h>
#include <qfile.h>
#include <qset.h>
#include <qmap.h>
#include <vector>

float GLWidget::MIN_Z = 150.0f;
//float GLWidget::MAX_Z = 11520.0f;
float GLWidget::MAX_Z = 2880.0f;

GLWidget::GLWidget(MyMainWindow* mainWin) : QGLWidget(QGLFormat(QGL::SampleBuffers), (QWidget*)mainWin) {
	this->mainWin = mainWin;

	roads = new RoadGraph();
	sketch = new Sketch();
	ref_roads = NULL;
	renderer = new RoadGraphRenderer();

	// set up the camera
	camera = new Camera();
	camera->setLookAt(0.0f, 0.0f, 0.0f);
	camera->setTranslation(0.0f, 0.0f, MAX_Z);

	// initialize the width of the road segments
	roads->widthBase = 12.0f;
}

GLWidget::~GLWidget() {
}

void GLWidget::drawScene() {
	// define the height of the roads according to the zoom factor
	float highwayHeight = camera->dz * 0.0012f;
	float avenueHeight = camera->dz * 0.0008f;

	// define the width of the road segment
	float widthBase;
	if (camera->dz < 1080.0f) {
		widthBase = camera->dz / 90.0f;
	} else if (camera->dz < 5760.0f) {
		widthBase = 12.0f;
	} else {
		widthBase = 24.0f;
	}

	// draw the road graph
	roads->generateMesh(
		widthBase,
		highwayHeight,
		avenueHeight,
		(camera->dz < MAX_Z / 4) ? 0.4f : 0.8f,
		(camera->dz < MAX_Z / 2) ? true : false);
	renderer->render(roads->renderables);

	// draw the sketch
	sketch->generateSketchMesh();
	renderer->render(sketch->renderables);

	// draw the reference road graph
	if (ref_roads != NULL) {
		ref_roads->generateMesh(
			widthBase,
			highwayHeight,
			avenueHeight,
			(camera->dz < MAX_Z / 4) ? 0.4f : 0.8f,
			(camera->dz < MAX_Z / 2) ? true : false,
			0.5f);
		renderer->render(ref_roads->renderables);
	}
}

/**
 * Start a new road from scratch.
 */
void GLWidget::newRoad() {
	roads->clear();

	updateGL();
}

/**
 * Open a road from the specified file.
 */
void GLWidget::openRoad(QString filename) {
	roads->clear();

	FILE* fp = fopen(filename.toUtf8().data(), "rb");
	roads->load(fp, 7);

	updateGL();
}

/**
 * Make a road graph from the sketch.
 */
void GLWidget::makeRoadsFromSketch() {
	RoadGraph* temp = sketch->makeRoads();
	sketch->clear();

	GraphUtil::mergeRoads(roads, temp);
	delete temp;

	updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	this->setFocus();

	lastPos = event->pos();

	if (event->buttons() & Qt::LeftButton) {
		QVector2D pos;
		mouseTo2D(event->x(), event->y(), &pos);
		mainWin->ui.statusBar->showMessage(QString("clicked (%1, %2)").arg(pos.x()).arg(pos.y()));

		if (mainWin->mode == MyMainWindow::MODE_VIEW) {
		} else if (mainWin->mode == MyMainWindow::MODE_SKETCH) {
			RoadVertexDesc v1_desc;
			if (!GraphUtil::getVertex(sketch, pos, 10.0f, v1_desc)) {
				// If there is a vertex close to the point, don't add new vertex. Otherwise, add a new vertex.
				RoadVertex* v1 = new RoadVertex(pos);
				v1_desc = boost::add_vertex(sketch->graph);
				sketch->graph[v1_desc] = v1;
			}

			// add the 2nd vertex of a line
			RoadVertex* v2 = new RoadVertex(pos);
			RoadVertexDesc v2_desc = boost::add_vertex(sketch->graph);
			sketch->graph[v2_desc] = v2;

			sketch->curVertex = v2_desc;

			// add an edge
			if (camera->dz <= MAX_Z / 2) {
				sketch->curEdge = GraphUtil::addEdge(sketch, v1_desc, v2_desc, 1, 1, false);
			} else {
				sketch->curEdge = GraphUtil::addEdge(sketch, v1_desc, v2_desc, 2, 2, false);
			}
		}

		updateGL();
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (mainWin->mode == MyMainWindow::MODE_SKETCH) {
		QVector2D pos;
		mouseTo2D(event->x(), event->y(), &pos);

		RoadVertexDesc v2_desc;
		if (GraphUtil::getVertex(sketch, pos, 10.0f, sketch->curVertex, v2_desc)) {
			// If there is a vertex close to the point, snap the point to the nearest vertex
			GraphUtil::snapVertex(sketch, sketch->curVertex, v2_desc);
		}
	}

	event->ignore();

	setCursor(Qt::ArrowCursor);
	updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	float dx = (float)(event->x() - lastPos.x());
	float dy = (float)(event->y() - lastPos.y());
	float camElevation = camera->getCamElevation();

	lastPos = event->pos();

	if (event->buttons() & Qt::LeftButton) {	
		if (mainWin->mode == MyMainWindow::MODE_VIEW) { // Shift the camera
			camera->changeXYZTranslation(-dx * camera->dz * 0.001f, dy * camera->dz * 0.001f, 0);
		} else if (mainWin->mode == MyMainWindow::MODE_SKETCH) { // Draw a sketch line
			QVector2D pos;
			mouseTo2D(event->x(), event->y(), &pos);

			GraphUtil::moveVertex(sketch, sketch->curVertex, pos);
		}
	} else if(event->buttons() & Qt::MidButton) {   // Shift the camera
		camera->changeXYZTranslation(-dx * camera->dz * 0.001f, dy * camera->dz * 0.001f, 0);
	} else if (event->buttons() & Qt::RightButton) { // Zoom the camera
		setCursor(Qt::SizeVerCursor);

		camera->changeXYZTranslation(0, 0, -dy * camera->dz * 0.02f);
		if (camera->dz < MIN_Z) camera->dz = MIN_Z;
		if (camera->dz > MAX_Z) camera->dz = MAX_Z;

		// define the width per lane according to the z coordinate of the camera
		if (camera->dz < 1080.0f) {
			roads->setWidth(camera->dz / 90.0f);
		} else if (camera->dz < 5760.0f) {
			roads->setWidth(12.0f);
		} else {
			roads->setWidth(24.0f);
		}

		lastPos = event->pos();

		mainWin->ui.statusBar->showMessage(QString("Z: %1").arg(camera->dz));
	}
	updateGL();
}

void GLWidget::initializeGL() {
	qglClearColor(QColor(233, 229, 220));
	glClearDepth(1.0f);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//---- lighting ----
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	static GLfloat lightPosition[4] = { 0.0f, 0.0f, 100.0f, 0.0f };
	static GLfloat lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat lightDiffuse[4] = { 0.75f, 0.75f, 0.5f, 1.0f };

	glEnable(GL_COLOR_MATERIAL);	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	static GLfloat materialSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat materialEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialEmission);
}

void GLWidget::resizeGL(int width, int height) {
	height = height ? height : 1;

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width/(GLfloat)height, 5.0f, 90000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera->applyCamTransform();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	camera->applyCamTransform();	

	drawScene();	
}

/**
 * Convert the screen space coordinate (x, y) to the model space coordinate.
 */
void GLWidget::mouseTo2D(int x,int y, QVector2D *result) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	// retrieve the matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// retrieve the projected z-buffer of the origin
	GLdouble origX, origY, origZ;
	gluProject(0, 0, 0, modelview, projection, viewport, &origX, &origY, &origZ);

	// set up the projected point
	GLfloat winX = (float)x;
	GLfloat winY = (float)viewport[3] - (float)y;
	GLfloat winZ = origZ;
	
	// unproject the image plane coordinate to the model space
	GLdouble posX, posY, posZ;
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	result->setX(posX);
	result->setY(posY);
}