#include "mypanelopengl.h"

#include <cmath>
#include <cstdio>
#include <stdlib.h>

#include <QtGui/QMouseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QString>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "platform.h"
#include "stopwatch.h"

#include "delaunay/li.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/delaunay.h"

#include "imagedata.h"

using cv::Mat;
using cv::imread;

using std::endl;
using std::string;
using std::vector;

using delaunay::LongInt;
using delaunay::MyPoint;
using delaunay::PointSetArray;



// TODO: the stopwatch code used makes code less readable.
static StopWatch globalSW;



void drawAPoint(double x,double y) {
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(0,0,0);
		glVertex2d(x,y);
	glEnd();
	glPointSize(1);
}

void drawALine(double x1,double y1, double x2, double y2) {
	glPointSize(1);
	glBegin(GL_LINE_LOOP);
	glColor3f(0,0,1);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
	glEnd();
	glPointSize(1);
}



void drawATriangle(double x1,double y1, double x2, double y2, double x3, double y3) {
	glBegin(GL_POLYGON);
	glColor3f(0,0.5,0);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		glVertex2d(x3,y3);
	glEnd();
}



void drawPointSetArray(const PointSetArray& pointSet) {
	// Draw input points
	for (int i = 1; i <= pointSet.noPt(); i++){
		LongInt px, py;
		pointSet.getPoint(i, px, py);
		drawAPoint(px.doubleValue(), py.doubleValue());
	}
}



// void drawDelaunayStuff() {
// 	// Draw all DAG leaf triangles.
// 	vector<TriRecord> leafNodes = dag.getLeafNodes();
//
// 	for (int i = 0; i < leafNodes.size(); i++){
// 		TriRecord leafNode = leafNodes[i];
//
// 		int pIndex1 = leafNode.vi_[0];
// 		int pIndex2 = leafNode.vi_[1];
// 		int pIndex3 = leafNode.vi_[2];
//
// 		// Ignore if from the super triangle (i.e. index too large for input set)
// 		if(pIndex1 > delaunayPointSet.noPt() - 3 ||
// 		   pIndex2 > delaunayPointSet.noPt() - 3 ||
// 		   pIndex3 > delaunayPointSet.noPt() - 3) {
// 			continue;
// 		}
//
// 		// Probably could clean this up..
// 		LongInt p1x, p1y, p2x, p2y, p3x, p3y;
//
// 		delaunayPointSet.getPoint(pIndex1, p1x, p1y);
// 		delaunayPointSet.getPoint(pIndex2, p2x, p2y);
// 		delaunayPointSet.getPoint(pIndex3, p3x, p3y);
//
//
// 		drawATriangle(p1x.doubleValue(), p1y.doubleValue(),
// 		              p2x.doubleValue(), p2y.doubleValue(),
// 		              p3x.doubleValue(), p3y.doubleValue());
// 		drawALine(p1x.doubleValue(), p1y.doubleValue(),
// 		          p2x.doubleValue(), p2y.doubleValue());
// 		drawALine(p2x.doubleValue(), p2y.doubleValue(),
// 		          p3x.doubleValue(), p3y.doubleValue());
// 		drawALine(p3x.doubleValue(), p3y.doubleValue(),
// 		          p1x.doubleValue(), p1y.doubleValue());
// 	}
// }



// DELAUNAY (it uses voronoiEdges)
void drawVoronoiPolygons(vector<PointSetArray>& voronoiPolys) {
	vector<PointSetArray>::iterator iter1;

	for (iter1 = voronoiPolys.begin(); iter1 != voronoiPolys.end(); ++iter1) {
		PointSetArray polygon = *iter1;

		for (int i = 1; i <= polygon.noPt(); i++) {
			int indexval;
			LongInt x1, y1, x2, y2;

			if (i+1 > polygon.noPt())
				indexval = 1;
			else
				indexval = i+1;

			polygon.getPoint(i,x1, y1);
			polygon.getPoint(indexval, x2, y2);
			drawALine(x1.doubleValue(), y1.doubleValue(),
			          x2.doubleValue(), y2.doubleValue());
		}
	}
}



void drawColoredPolygons(const vector<ColoredPolygon>& renderedPolygons) {
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_SCISSOR_TEST);

	for (unsigned int i = 0; i < renderedPolygons.size(); i++) {
		ColoredPolygon coloredPoly = renderedPolygons[i];

		glBegin(GL_POLYGON);
		glColor3fv(coloredPoly.rgb); // rgb?

		for(unsigned int j = 0; j < coloredPoly.poly.size() / 2; j++) {
			double x = coloredPoly.poly[2 * j];
			double y = coloredPoly.poly[2 * j + 1];

			glVertex2d(x, y);
		}

		glEnd();
	}

	glDisable(GL_SCISSOR_TEST);
}



// XXX this should be a method?
void refreshProjection(int width, int height,
                       int& canvas_offsetX, int& canvas_offsetY,
                       ImageData *imData) {
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	canvas_offsetX = 0;
	canvas_offsetY = 0;

	// If we haven't loaded an image,
	// we don't particularly care what the coord system is.
	if (imData == NULL) {
		// Just some boring thing.
		glOrtho(-1, 1, 1, -1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return;
	}

	int imWidth = imData->width();
	int imHeight = imData->height();

	double imageRatio = ((double) imWidth) / imHeight;
	double windowRatio = ((double) width) / height;

	if (imageRatio > windowRatio) {
		double ratio = ((double) width) / height;

		int renderWidth = imWidth;
		int renderHeight = (int) (imWidth / ratio);

		int delta = (renderHeight - imHeight) / 2;

		glOrtho(0,
		        renderWidth,
		        imHeight + delta,
		        -delta,
		        -1,
		        1);

		// Scissor test to draw stuff only within the image
		// (Use this for the voronoi-diagram-colors
		int scissorDelta = delta * height / renderHeight;
		canvas_offsetY = scissorDelta;
		glScissor(0, scissorDelta, width, height - (2 * scissorDelta));
	} else {
		double ratio = ((double) width) / height;

		int renderWidth = (int) (imHeight * ratio);
		int renderHeight = imHeight;

		int delta = (renderWidth - imWidth) / 2;

		glOrtho(-delta,
		        imWidth + delta,
		        renderHeight,
		        0,
		        -1,
		        1);

		// Scissor test to draw stuff only within the image
		// (Use this for the voronoi-diagram-colors
		int scissorDelta = delta * width / renderWidth;
		canvas_offsetX = scissorDelta;
		glScissor(scissorDelta, 0, width - (2 * scissorDelta), height);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// Uses OpenCV
ImageData* loadImageData(string imgFilename) {
	Mat imgMat = imread(imgFilename.c_str()); // BGR

	return new ImageData(imgMat, CV_BGR2RGB);
}



// POLYREP:INTVEC
// also uses polypixel's ColoredPolygon
vector<ColoredPolygon> generateColoredPolygons(vector< vector<int> >& polys, const ImageData& imData) {
	vector<ColoredPolygon> renderedPolygons;

	StopWatch allSW;
	allSW.resume();

	for (unsigned int i = 0; i < polys.size(); i++) {
		// Clip polygon to ensure we have nothing out of bounds
		// vector<int> unclippedPoly = polys[i];
		// vector<int> poly = clipPolygonToRectangle(unclippedPoly, 0, 0, loadedImageWidth, loadedImageHeight);
		vector<int> poly = polys[i];

		// TODO: Would be nice to be able to inject another function
		// instead of `findSomeColor3iv`.
		int colorIv[3];
		findSomeColor3iv(imData, poly, colorIv);

		// XXX following could be a method / ctor, right?
		// ColoredPolygon (from polypixel),
		// internally uses POLYREP:INTVEC
		ColoredPolygon coloredPoly;

		coloredPoly.poly = poly;
		coloredPoly.rgb[0] = (float) colorIv[0] / 255;
		coloredPoly.rgb[1] = (float) colorIv[1] / 255;
		coloredPoly.rgb[2] = (float) colorIv[2] / 255;

		renderedPolygons.push_back(coloredPoly);
	}

	allSW.pause();
	double timeFindSomeColor = allSW.ms();
	int n = polys.size();
	double timeAvg = timeFindSomeColor / n;
	qDebug("TIME: Average: %f for %d polygons. Total: %f", timeAvg, n, timeFindSomeColor);

	return renderedPolygons;
}



// POLYREP:MYPOINTVEC => :INTVEC
vector<ColoredPolygon> generateColoredPolygons(vector< vector<MyPoint> >& myPointPolys, const ImageData& imData) {
	// Coerce the PSAs to vec<int> poly representation
	vector< vector<int> > ivPolys;

	for (unsigned int i = 0; i < myPointPolys.size(); i++) {
		vector<MyPoint> mpPoly = myPointPolys[i];
		vector<int> poly;

		for (unsigned int ptIdx = 0; ptIdx < mpPoly.size(); ptIdx++) {
			MyPoint pt = mpPoly[ptIdx];
			poly.push_back((int) pt.x.doubleValue());
			poly.push_back((int) pt.y.doubleValue());
		}

		ivPolys.push_back(poly);
	}

	return generateColoredPolygons(ivPolys, imData);
}



// POLYREP:POINTSETARRAY => :INTVEC
vector<ColoredPolygon> generateColoredPolygons(vector<PointSetArray>& psas, const ImageData& imData) {
	// Coerce the PSAs to vec<int> poly representation
	vector< vector<int> > ivPolys;

	for (unsigned int i = 0; i < psas.size(); i++) {
		ivPolys.push_back(coercePSAPolyToIVecPoly(psas[i]));
	}

	return generateColoredPolygons(ivPolys, imData);
}



// DELAUNAY & VORONOI
void MyPanelOpenGL::insertPoint(LongInt x, LongInt y) {
	// DELAUNAY
	inputPointSet_.addPoint(x, y);
}



MyPanelOpenGL::MyPanelOpenGL(QWidget *parent) : QGLWidget (parent) {
}



void MyPanelOpenGL::initializeGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}



void MyPanelOpenGL::resizeGL(int width, int height) {
	refreshProjection(width, height, canvasOffsetX_, canvasOffsetY_, imData_);
}



void MyPanelOpenGL::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	switch (currentRenderType_) {
		case EFFECT:
			drawColoredPolygons(renderedPolygons_);
			break;

		case EDGE_RAW:
			pdfTextures_.edgesTexture->renderPlane();
			break;

		case EDGE_SHARP:
			pdfTextures_.edgesSharpTexture->renderPlane();
			break;

		case EDGE_BLUR:
			pdfTextures_.edgesBlurTexture->renderPlane();
			break;

		case PDF:
			pdfTextures_.pdfTexture->renderPlane();
			break;

		case IMAGE:
			imData_->renderPlane();
			break;

		default:
		case NONE:
			break;
	}

	if (showVoronoiEdges_) {
		// DELAUNAY (voronoiEdges)
		drawVoronoiPolygons(voronoiPolygons_);
	}

	if (showVoronoiSites_) {
		drawPointSetArray(inputPointSet_);
	}

	glPopMatrix();
}



void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {
	//qDebug("Window: %d, %d\n", event->x(), event->y());
	if (!hasLoadedImage()) {
		return;
	}

	int loadedImageWidth = imData_->width();
	int loadedImageHeight = imData_->height();

	QSize widgetSize = size();
	int windowWidth = widgetSize.width();
	int windowHeight = widgetSize.height();

	double imageRatio = ((double) loadedImageWidth) / loadedImageHeight;
	double windowRatio = ((double) windowWidth) / windowHeight;

	int renderWidth = 2;
	int renderHeight = 2;

	int deltaX = 1;
	int deltaY = 1;

	if (loadedImageWidth > 0) {
		if (imageRatio > windowRatio) {
			double ratio = ((double) windowWidth) / windowHeight;

			renderWidth = loadedImageWidth;
			renderHeight = (int) (loadedImageWidth / ratio);

			deltaY = (renderHeight - loadedImageHeight) / 2;
		} else {
			double ratio = ((double) windowWidth) / windowHeight;

			renderWidth = (int) (loadedImageHeight * ratio);
			renderHeight = loadedImageHeight;

			deltaX = (renderWidth - loadedImageWidth) / 2;
		}

		double viewScale = (double) renderWidth / windowWidth;

		int px = (event->x() * viewScale) - deltaX;
		int py = (event->y() * viewScale) - deltaY;

		qDebug("Insert Point: %d, %d\n", px, py);

		insertPoint(px, py);
		updateNumPoints(inputPointSet_.noPt());

		updateGL();
	}
}



void MyPanelOpenGL::doVoronoiDiagram() {
	//qDebug("Do Voronoi creation\n");

	StopWatch voroSW;

	voroSW.reset();
	voroSW.resume();

	if (algorithm_ == kDelaunayAlgorithm) {
		// DELAUNAY
		qDebug("Do doDelaunay in MPOG::doVoronoi");

		voronoiPolygons_ = delaunay::runDelaunayAlgorithm(inputPointSet_);
	} else {
		// VORONOI
		voronoiPolygons_ = voronoi::runVoronoiAlgorithm(inputPointSet_);
	}


	// Make the colored polygons from Voronoi.
	assert(imData_ != NULL);
	renderedPolygons_ = generateColoredPolygons(voronoiPolygons_, *imData_);
	currentRenderType_ = EFFECT;

	voroSW.pause();
	double timePolyColor = voroSW.ms();
	qDebug("TIME: generateColoredPolygons(..) is %f", timePolyColor);
	voroSW.reset();
	voroSW.resume();

	setVoronoiComputed(true);

	updateGL();
}



void MyPanelOpenGL::doOpenImage() {
	//get a filename to open
	QString qStr_fileName =
		QFileDialog::getOpenFileName(Q_NULLPTR,
	                                 tr("Open Image"),
	                                 ".",
	                                 tr("Image Files (*.png *.jpg *.bmp)"));
	if (qStr_fileName == "") {
		return;
	}

	string filenameStr = qStr_fileName.toStdString();

	emit updateFilename(qStr_fileName); // to Qt textbox

	imData_ = loadImageData(filenameStr);
	loadedImageFilename_ = filenameStr;

	QSize widgetSize = size();
	refreshProjection(widgetSize.width(), widgetSize.height(),
	                  canvasOffsetX_, canvasOffsetY_,
	                  imData_);
	emit imageLoaded();

	currentRenderType_ = IMAGE;
	updateGL();
}



void MyPanelOpenGL::doSaveImage() {
	// TODO: This impl. feels impure
	// TODO: Should be a way to set output filename
	// TODO: ImageData stuff applies here, too.
	string outputImageFilename = "output.bmp";

	int x = canvasOffsetX_;
	int y = canvasOffsetY_;

	QSize widgetSize = size();
	int windowWidth = widgetSize.width();
	int windowHeight = widgetSize.height();

	int copyWidth = (windowWidth - (2 * x));
	int copyHeight = (windowHeight - (2 * y));

	int numComponents = 3; // RGB

	// Memory for width * height * RGB pixel values.
	unsigned char *data;
	data = (unsigned char *) malloc(numComponents * copyWidth * copyHeight * sizeof(unsigned char));

	glPixelStorei(GL_PACK_ALIGNMENT, 1); // align to the byte..
	glReadBuffer(GL_FRONT);

	// Read in "correct" row...
	// (I think I have to do this since the texture coordinates' y-axis is flipped
	//  compared to the rendered y-axis).

	for (int rowOffset = 0; rowOffset < copyHeight; rowOffset++) {
		int copyRow = y + copyHeight - rowOffset - 1; // reverse this.
		unsigned char * copyAddress = data + rowOffset * (copyWidth * numComponents);
		glReadPixels(x, copyRow, copyWidth, 1, GL_BGR, GL_UNSIGNED_BYTE, copyAddress);
	}

	Mat img(copyHeight, copyWidth, CV_8UC3, data);
	imwrite(outputImageFilename.c_str(), img);
}



void MyPanelOpenGL::doDrawImage() {
	currentRenderType_ = IMAGE;
	updateGL();
}



void MyPanelOpenGL::doDrawEdge() {
	currentRenderType_ = EDGE_RAW;
	updateGL();
}



void MyPanelOpenGL::doDrawEdgeSharp() {
	currentRenderType_ = EDGE_SHARP;
	updateGL();
}



void MyPanelOpenGL::doDrawEdgeBlur() {
	currentRenderType_ = EDGE_BLUR;

	updateGL();
}



void MyPanelOpenGL::doDrawPDF() {
	currentRenderType_ = PDF;
	updateGL();
}



void MyPanelOpenGL::doDrawEffect() {
	currentRenderType_ = EFFECT;
	updateGL();
}



void MyPanelOpenGL::doGenerateUniformRandomPoints() {
	if (!hasLoadedImage()) return;

	// POINTREP:INTVEC
	// Returns {x0, y0, x1, y1,...}
	int width = imData_->width();
	int height = imData_->height();
	vector<int> points = generateUniformRandomPoints(width, height, numPDFPoints_);

	for (unsigned int i = 0; i < points.size() / 2; i++) {
		int x = points[i * 2];
		int y = points[i * 2 + 1];

		insertPoint(x, y);
	}

	updateNumPoints(inputPointSet_.noPt());

	updateGL();
}



void MyPanelOpenGL::doPDF() {
	// POINTREP:INTVEC
	// Returns {x0, y0, x1, y1,...}
	vector<int> points = generatePointsWithPDF(loadedImageFilename_, numPDFPoints_, &pdfTextures_);

	for (unsigned int i = 0; i < points.size() / 2; i++) {
		int x = points[i * 2];
		int y = points[i * 2 + 1];

		insertPoint(x, y);
	}

	updateNumPoints(inputPointSet_.noPt());
	setUsePDF(true);

	updateGL();
}



void MyPanelOpenGL::clearAll() {
	// Clear all our points, and such data.

	// Clear all the colored polygons.
	renderedPolygons_.clear();

	voronoiPolygons_.clear();

	// Clear all the points.
	inputPointSet_.eraseAllPoints();

	// Signals and stuff
	updateNumPoints(inputPointSet_.noPt());
	setUsePDF(false);
	setVoronoiComputed(false);

	currentRenderType_ = NONE;

	updateGL();
}



void MyPanelOpenGL::mouseMoveEvent(QMouseEvent *) {
}



void MyPanelOpenGL::setShowVoronoiSites(bool b) {
	showVoronoiSites_ = b;
	updateGL();
}



void MyPanelOpenGL::setShowVoronoiEdges(bool b) {
	showVoronoiEdges_ = b;
	updateGL();
}



void MyPanelOpenGL::setNumPoints1k() {
	setNumPoints(1000);
}



void MyPanelOpenGL::setNumPoints5k() {
	setNumPoints(5000);
}



void MyPanelOpenGL::setNumPoints(int n) {
	numPDFPoints_ = n;
	updateNumPointsToGenerate(n);
}



void MyPanelOpenGL::useDelaunayAlgorithm() {
	algorithm_ = kDelaunayAlgorithm;
}



void MyPanelOpenGL::useVoronoiAlgorithm() {
	algorithm_ = kVoronoiAlgorithm;
}



void MyPanelOpenGL::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
		case Qt::Key_Escape:
			close();
			break;
		default:
			event->ignore();
			break;
	}
}

