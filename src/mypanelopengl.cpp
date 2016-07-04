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
#include "stopWatch.h"

#include "li.h"
#include "lmath.h"
#include "pointSet.h"
#include "pointSetArray.h"
#include "trist.h"
#include "delaunayTri.h"
#include "directedGraph.h"
#include "delaunay.h"

#include "imagedata.h"
#include "polypixel.h"
#include "generateProbabilityDistribution.h"

//Imports as part of implementing Fortune's algorithm
#include "Voronoi.h"
#include "VPoint.h"

using cv::Mat;
using cv::imread;

using std::string;
using std::vector;



// DELAUNAY
PointSetArray inputPointSet;
// PointSetArray delaunayPointSet; // Add the super triangle stuff to this.

static StopWatch globalSW;

// Variables for Image Logic.
string loadedImageFilename = "";
ImageData *imData = NULL;

GLuint loadedImageTexture;

PDFTextures pdfTextures_;

// Variables for colored polygons
bool hasCalculatedColoredPolygons = 0;
vector<ColoredPolygon> renderedPolygons;
string imageName;

// Variables for render state stuff.
enum ShowImageType { IMAGE, EDGE_RAW, EDGE_SHARP, EDGE_BLUR, PDF, EFFECT };
ShowImageType currentRenderType = IMAGE;
bool showVoronoiSites = true;
bool showVoronoiEdges = false;

// Variables for point generation.
int numPDFPoints = 75;

//Initialize voronoi components for Fortune's algorithm
vor::Vertices * voronoiVertices_ = new vor::Vertices();



bool hasLoadedImage() {
	return imData != NULL;
}

// These three functions are for those who are not familiar with OpenGL, you can change these or even completely ignore them

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



void drawPlaneUsingTexture(GLuint tex) {
	// If not loaded, don't render..
	if (!hasLoadedImage()) { return; }

	qDebug("Draw the texture");
	glEnable(GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, tex);

	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (0.0, 0.0, -1.0);

		glTexCoord2f (1.0, 0.0);
		glVertex3f (imData->width(), 0.0, -1.0);

		glTexCoord2f (1.0, 1.0);
		glVertex3f (imData->width(), imData->height(), -1.0);

		glTexCoord2f (0.0, 1.0);
		glVertex3f (0.0, imData->height(), -1.0);
	glEnd ();

	glDisable(GL_TEXTURE_2D);
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
// used in display()
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



void drawColoredPolygons() {
	if (!hasCalculatedColoredPolygons) {
		return;
	}

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_SCISSOR_TEST);

	for (int i = 0; i < renderedPolygons.size(); i++) {
		ColoredPolygon coloredPoly = renderedPolygons[i];

		glBegin(GL_POLYGON);
		glColor3fv(coloredPoly.rgb); // rgb?

		for(int j = 0; j < coloredPoly.poly.size() / 2; j++) {
			double x = coloredPoly.poly[2 * j];
			double y = coloredPoly.poly[2 * j + 1];

			glVertex2d(x, y);
		}

		glEnd();
	}

	glDisable(GL_SCISSOR_TEST);
}



void display(vector<PointSetArray> voronoiPolys) {
	switch (currentRenderType) {
		case EFFECT:
			drawColoredPolygons();
			break;

		case EDGE_RAW:
			drawPlaneUsingTexture(pdfTextures_.edgesTexture);
			break;

		case EDGE_SHARP:
			drawPlaneUsingTexture(pdfTextures_.edgesSharpTexture);
			break;

		case EDGE_BLUR:
			drawPlaneUsingTexture(pdfTextures_.edgesBlurTexture);
			break;

		case PDF:
			drawPlaneUsingTexture(pdfTextures_.pdfTexture);
			break;

		case IMAGE:
		default:
			drawPlaneUsingTexture(loadedImageTexture);
			break;
	}

	if (showVoronoiEdges) {
		// DELAUNAY (voronoiEdges)
		drawVoronoiPolygons(voronoiPolys);
	}


	if (showVoronoiSites) {
		// DELAUNAY (inputPointSet vs voronoisites)
		// Point indices are 1-based here
		// Draw input points
		for (int i = 1; i <= inputPointSet.noPt(); i++){
			LongInt px, py;
			inputPointSet.getPoint(i, px, py);
			drawAPoint(px.doubleValue(), py.doubleValue());
		}
	}
}



// POLYREP:INTVEC
// also uses polypixel's ColoredPolygon
void generateColoredPolygons(vector< vector<int> >& polys) {
	hasCalculatedColoredPolygons = 0;
	renderedPolygons.clear();

	StopWatch allSW;
	allSW.resume();

	for (int i = 0; i < polys.size(); i++) {
		// Clip polygon to ensure we have nothing out of bounds
		// vector<int> unclippedPoly = polys[i];
		// vector<int> poly = clipPolygonToRectangle(unclippedPoly, 0, 0, loadedImageWidth, loadedImageHeight);
		vector<int> poly = polys[i];

		// TODO: Would be nice to be able to inject another function
		// instead of `findSomeColor3iv`.
		int colorIv[3];
		findSomeColor3iv(*imData, poly, colorIv);

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

	hasCalculatedColoredPolygons = 1;
}



// POLYREP:MYPOINTVEC => :INTVEC
void generateColoredPolygons(vector< vector<MyPoint> >& myPointPolys) {
	// Coerce the PSAs to vec<int> poly representation
	vector< vector<int> > ivPolys;

	for (int i = 0; i < myPointPolys.size(); i++) {
		vector<MyPoint> mpPoly = myPointPolys[i];
		vector<int> poly;

		for (int ptIdx = 0; ptIdx < mpPoly.size(); ptIdx++) {
			MyPoint pt = mpPoly[ptIdx];
			poly.push_back((int) pt.x.doubleValue());
			poly.push_back((int) pt.y.doubleValue());
		}

		ivPolys.push_back(poly);
	}

	generateColoredPolygons(ivPolys);
}



// POLYREP:POINTSETARRAY => :INTVEC
void generateColoredPolygons(vector<PointSetArray>& psas) {
	// Coerce the PSAs to vec<int> poly representation
	vector< vector<int> > ivPolys;

	for (int i = 0; i < psas.size(); i++) {
		ivPolys.push_back(coercePSAPolyToIVecPoly(psas[i]));
	}

	generateColoredPolygons(ivPolys);
}



// DELAUNAY
// NB: unused, but also, genColorPoly() is in ::doVoronoiDiagram
// void generateDelaunayColoredPolygons() {
// 	qDebug("Calculate colored");
//
// 	// Pre-condition: 'dag' generated.
//
// 	vector< vector<MyPoint> > polys;
//
// 	// Create vector of vector of points, from delaunay.
// 	// Draw all DAG leaf triangles.
// 	vector<TriRecord> leafNodes = dag.getLeafNodes();
// 	for (int i = 0; i < leafNodes.size(); i++){
// 		TriRecord leafNode = leafNodes[i];
//
// 		int pIndex1 = leafNode.vi_[0];
// 		int pIndex2 = leafNode.vi_[1];
// 		int pIndex3 = leafNode.vi_[2];
//
// 		// Ignore if from the super triangle (i.e. index too large for input set)
// 		if (pIndex1 > delaunayPointSet.noPt() - 3 ||
// 		    pIndex2 > delaunayPointSet.noPt() - 3 ||
// 		    pIndex3 > delaunayPointSet.noPt() - 3) {
// 			continue;
// 		}
//
// 		// Probably could clean this up..
// 		// Since MyPoint is now exposed.
// 		LongInt p1x, p1y, p2x, p2y, p3x, p3y;
//
// 		delaunayPointSet.getPoint(pIndex1, p1x, p1y);
// 		delaunayPointSet.getPoint(pIndex2, p2x, p2y);
// 		delaunayPointSet.getPoint(pIndex3, p3x, p3y);
//
// 		// Add a polygon for the triangle.
// 		vector<MyPoint> poly;
// 		poly.push_back(MyPoint(p1x, p1y));
// 		poly.push_back(MyPoint(p2x, p2y));
// 		poly.push_back(MyPoint(p3x, p3y));
//
// 		polys.push_back(poly);
// 	}
//
// 	generateColoredPolygons(polys);
// }



// XXX this should be a method
void refreshProjection(int width, int height,
                       int& canvas_offsetX, int& canvas_offsetY) {
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	canvas_offsetX = 0;
	canvas_offsetY = 0;

	// If we haven't loaded an image,
	// we don't particularly care what the coord system is.
	if (!hasLoadedImage()) {
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



void init(void) {
	glClearColor (1.0,1.0,1.0, 1.0);
}



// DELAUNAY & VORONOI
void tryInsertPoint(LongInt x, LongInt y) {
	// DELAUNAY
	int ptIndex = inputPointSet.addPoint(x, y);

	// VORONOI
	VPoint *vp = new VPoint(x.doubleValue()+((double)rand()*15.0/(double)RAND_MAX),
	                        y.doubleValue()+((double)rand()*15.0/(double)RAND_MAX));
	voronoiVertices_->push_back(vp);
	//voronoivertices ->push_back(new VPoint(x.doubleValue(), y.doubleValue() ));
}



void loadOpenGLTextureFromFilename(string imgFilename) {
	//string loadedImageFilename = "";
	//int loadedImageWidth = -1;
	//int loadedImageHeight = -1;
	//GLuint loadedImageTexture;

	// See http://open.gl/textures for more information.

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &loadedImageTexture);
	glBindTexture(GL_TEXTURE_2D, loadedImageTexture);


	// loadedImageData should be in RGB format, from
	// imgFilename.c_str() filetype.
	Mat src = imread(imgFilename.c_str()); // BGR
	cvtColor(src, src, CV_BGR2RGB);
	int dataSize = 3 * src.cols * src.rows * sizeof(unsigned char);
	unsigned char* dataCpy = (unsigned char*) malloc(dataSize);
	memcpy(dataCpy, src.data, dataSize);
	imData = new ImageData(dataCpy,
	                       src.cols,
	                       src.rows);
	// TODO: Should free imData at some point..
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_RGB,
	             imData->width(),
	             imData->height(),
	             0,
	             GL_RGB,
	             GL_UNSIGNED_BYTE,
	             imData->data());
	loadedImageFilename = imgFilename;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
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
	refreshProjection(width, height, canvasOffsetX_, canvasOffsetY_);
}



void MyPanelOpenGL::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	display(voronoiPolygons_);

	glPopMatrix();
}



void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {
	//qDebug("Window: %d, %d\n", event->x(), event->y());
	if (!hasLoadedImage()) {
		return;
	}

	int loadedImageWidth = imData->width();
	int loadedImageHeight = imData->height();

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

		tryInsertPoint(px, py);
		updateNumPoints(inputPointSet.noPt());

		updateGL();
	}
}



// This function is part of Fortune's implementation
// & outputs the voronoiEdges required for the generateColoredPolygons
//
// VORONOI, wrapper to voronoiEdges(???)
vector<PointSetArray> createPolygonsFortune(vor::Edges *voronoiedges) {
	vector<PointSetArray> voronoiEdges;

	// The dictionary is indexed by the voronoi points and gives the polygon for each voronoi.
	// The border polygons are unbounded, so need to be careful.
	std::map< VPoint *, vector<VEdge *> > dictionary;

	// XXX This should be its own function.
	// Given a list of VEdges,
	// construct reverse-lookup of VEdges associated with each VPoint.
	for (vor::Edges::iterator i = voronoiedges->begin(); i != voronoiedges->end(); ++i) {
		VEdge *edge = *i;
		VPoint *leftPt = edge->left;
		VPoint *rightPt = edge->right;

		//Check if the dictionary has leftpoint
		std::map< VPoint *, vector<VEdge *> >::iterator it = dictionary.find(leftPt);

		if (it != dictionary.end()) {
			it->second.push_back(edge);
		} else {
			vector<VEdge *> listofedges;
			listofedges.push_back(edge);
			dictionary.insert(std::map<VPoint *,vector<VEdge *> >::value_type(leftPt,listofedges));
		}

		//Check if the dictionary has rightpoint
		std::map< VPoint *, vector<VEdge *> >::iterator it2 = dictionary.find(rightPt);

		if (it2 != dictionary.end()) {
			it2->second.push_back(edge);
		} else {
			vector<VEdge *> listofedges2;
			listofedges2.push_back(edge);
			dictionary.insert(std::map<VPoint *,vector<VEdge *> >::value_type(rightPt,listofedges2));
		}
	}


	// Convert each of the dictionary values (unsorted edges associated with vertex)
	// into ordered list of PointSetArray vertex set.
	std::map< VPoint *, vector<VEdge *> >::iterator dictioniter;

	for (dictioniter = dictionary.begin(); dictioniter != dictionary.end(); ++dictioniter) {
		vector<VEdge *> polygonEdges = dictioniter->second;
		PointSetArray polygonvertices;
		vector<VPoint *> revvector;

		// polygonEdges has a set of edges, create an ordered list of points from this.
		// Take the first edge, store its start and end points into pointsetarray.
		VEdge * firstedge = *polygonEdges.begin();
		polygonvertices.addPoint( (LongInt)firstedge->start->x, (LongInt)firstedge->start->y );
		polygonvertices.addPoint( (LongInt)firstedge->end->x, (LongInt)firstedge->end->y );

		// Store the start and end point of this edge for later use.
		// When the last edge is found, its end edge will be the same as
		// the first edge's starting point. This signifies completion of polygon.
		//
		// The ending point will be updated as each subsequent edge is found.
		VPoint *startpoint = new VPoint(firstedge->start->x, firstedge->start->y);
		VPoint *endpoint   = new VPoint(firstedge->end->x, firstedge->end->y); // XXX CpyCtor
		polygonEdges.erase(polygonEdges.begin()); // NB remove firstedge, as we've consumed it

		int edgeCount = polygonEdges.size(); // Why would *this* matter? WRONG, surely.


		while (edgeCount > 0) {
			vector<VEdge *>::iterator polygonedgeiter;
			int exitflag = 0;

			// Iterate throught the remaining list of edges to find the subsequent edge
			for (polygonedgeiter = polygonEdges.begin(); polygonedgeiter != polygonEdges.end(); ++polygonedgeiter) {
				VEdge * eachedge = *polygonedgeiter;

				if (eachedge->start->x == endpoint->x &&
				    eachedge->start->y == endpoint->y) {
					polygonvertices.addPoint( (LongInt)eachedge->end->x, (LongInt)eachedge->end->y );
					endpoint->x = eachedge->end->x; // Check if this pointer assignment works...
					endpoint->y = eachedge->end->y; // XXX Copy-Assignment operator
					polygonEdges.erase(polygonedgeiter); // Delete the edge that has been added to the pointset array
					edgeCount--;
					exitflag = 1;

					break;
				} else if (eachedge->end->x == endpoint->x &&
				           eachedge->end->y == endpoint->y) {
					polygonvertices.addPoint( (LongInt)eachedge->start->x, (LongInt)eachedge->start->y );
					endpoint->x = eachedge->start->x;
					endpoint->y = eachedge->start->y;
					polygonEdges.erase(polygonedgeiter); // Delete the edge that has been added to the pointset array
					edgeCount--;
					exitflag = 1;

					break;
				}
			}

			//If edge has not been found, the following break executes and while exits with incomplete polygon(border condition).
			if (edgeCount > 0 && exitflag == 0) {
				//PointSetArray revpolygonvertices;
				for (polygonedgeiter = polygonEdges.begin(); polygonedgeiter != polygonEdges.end(); ++polygonedgeiter) {
					VEdge * eachedge = *polygonedgeiter;

					if (eachedge->start->x == startpoint->x &&
					    eachedge->start->y == startpoint->y) {
						//revpolygonvertices.addPoint( (LongInt)eachedge->end->x, (LongInt)eachedge->end->y );
						revvector.push_back(eachedge->end);
						startpoint->x = eachedge->end->x; // Check if this pointer assignment works...
						startpoint->y = eachedge->end->y;
						polygonEdges.erase(polygonedgeiter);
						edgeCount--;
						exitflag = 1;

						break;
					} else if (eachedge->end->x == startpoint->x &&
					           eachedge->end->y == startpoint->y) {
						//revpolygonvertices.addPoint( (LongInt)eachedge->start->x, (LongInt)eachedge->start->y );
						revvector.push_back(eachedge->start);
						startpoint->x = eachedge->start->x;
						startpoint->y = eachedge->start->y;
						polygonEdges.erase(polygonedgeiter);
						edgeCount--;
						exitflag = 1;

						break;
					}
				} // End of for
			}
		} //End of while- Current polygon has been found

		//Push the polygon into the list of polygons.
		while (revvector.size() != 0) {
			VPoint * point = revvector.back();
			polygonvertices.addPoint( (LongInt)point->x, (LongInt)point->y );
			revvector.pop_back();
		}

		voronoiEdges.push_back(polygonvertices);
	}// All polygons have been found

	return voronoiEdges;
}



void MyPanelOpenGL::doVoronoiDiagram() {
	//qDebug("Do Voronoi creation\n");

	StopWatch voroSW;

	voroSW.reset();
	voroSW.resume();

	bool useOldVoronoiAlgo = false;

	if (useOldVoronoiAlgo) {
		// DELAUNAY

		DirectedGraph dag = dagFromInputPoints(inputPointSet);

		qDebug("Do doDelaunay in MPOG::doVoronoi\n");
		tryDelaunayTriangulation(dag);
		//generateDelaunayColoredPolygons(); // too slow.

		updateGL();

		voroSW.pause();
		double timeDelaunay = voroSW.ms();
		qDebug("TIME: doDelaunayTriangulation() is %f", timeDelaunay);
		voroSW.reset();
		voroSW.resume();

		voronoiPolygons_ = createVoronoi(dag); // in `delaunay`

		voroSW.pause();
		double timeCreateVoronoi = voroSW.ms();
		qDebug("TIME: createVoronoi() is %f", timeCreateVoronoi);
		voroSW.reset();
		voroSW.resume();
	} else {
		// VORONOI

		// Bounding Points for Fortune's
		voronoiVertices_->push_back(new VPoint(-10000.0 + ((double)rand()*15.0 / (double)RAND_MAX),
		                                        10000.0 + ((double)rand()*15.0 / (double)RAND_MAX)));
		voronoiVertices_->push_back(new VPoint( 10000.0 + ((double)rand()*15.0 / (double)RAND_MAX),
		                                        10000.0 + ((double)rand()*15.0 / (double)RAND_MAX)));
		voronoiVertices_->push_back(new VPoint( 10000.0 + ((double)rand()*15.0 / (double)RAND_MAX),
		                                       -10000.0 + ((double)rand()*15.0 / (double)RAND_MAX)));
		voronoiVertices_->push_back(new VPoint(-10000.0 + ((double)rand()*15.0 / (double)RAND_MAX),
		                                       -10000.0 + ((double)rand()*15.0 / (double)RAND_MAX) ));

		vor::Voronoi *voronoi;
		vor::Edges *voronoiEdges = voronoi->getEdges(voronoiVertices_, 10000, 10000);

		voroSW.pause();
		double timeFortune = voroSW.ms();
		qDebug("TIME: voronoi->GetEdges(..) is %f", timeFortune);
		voroSW.reset();
		voroSW.resume();

		voronoiPolygons_ = createPolygonsFortune(voronoiEdges);

		voroSW.pause();
		double timePolyConstruct = voroSW.ms();
		qDebug("TIME: createPolygonsFortune() is %f", timePolyConstruct);
		voroSW.reset();
		voroSW.resume();
	}


	// Make the colored polygons from Voronoi.
	generateColoredPolygons(voronoiPolygons_);
	currentRenderType = EFFECT;

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
	string filenameStr = qStr_fileName.toStdString();
	imageName = filenameStr;

	qDebug(filenameStr.c_str());

	updateFilename(qStr_fileName); // to Qt textbox
	loadOpenGLTextureFromFilename(filenameStr);

	QSize widgetSize = size();
	refreshProjection(widgetSize.width(), widgetSize.height(),
	                  canvasOffsetX_, canvasOffsetY_);
	imageLoaded();
}



void MyPanelOpenGL::doSaveImage() {
	// TODO: This impl. feels impure
	// TODO: Should be a way to set output filename
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
	qDebug("Draw OpenGL Image");
	currentRenderType = IMAGE;

	updateGL();
}



void MyPanelOpenGL::doDrawEdge() {
	qDebug("Draw Edge");
	currentRenderType = EDGE_RAW;

	updateGL();
}



void MyPanelOpenGL::doDrawEdgeSharp() {
	qDebug("Draw Edge (Sharp)");
	currentRenderType = EDGE_SHARP;

	updateGL();
}



void MyPanelOpenGL::doDrawEdgeBlur() {
	qDebug("Draw Edge (Blur)");
	currentRenderType = EDGE_BLUR;

	updateGL();
}



void MyPanelOpenGL::doDrawPDF() {
	qDebug("Draw PDF");
	currentRenderType = PDF;

	updateGL();
}



void MyPanelOpenGL::doDrawEffect() {
	qDebug("Draw Effect");
	currentRenderType = EFFECT;

	updateGL();
}



void MyPanelOpenGL::doGenerateUniformRandomPoints() {
	if (imData == NULL) return;

	// POINTREP:INTVEC
	// Returns {x0, y0, x1, y1,...}
	int width = imData->width();
	int height = imData->height();
	vector<int> points = generateUniformRandomPoints(width, height, numPDFPoints);

	for (int i = 0; i < points.size() / 2; i++) {
		int x = points[i * 2];
		int y = points[i * 2 + 1];


		tryInsertPoint(x, y);
	}

	updateNumPoints(inputPointSet.noPt());
	setUsePDF(true);

	updateGL();
}



void MyPanelOpenGL::doPDF() {
	// POINTREP:INTVEC
	// Returns {x0, y0, x1, y1,...}
	vector<int> points = generatePointsWithPDF(loadedImageFilename, numPDFPoints, &pdfTextures_);

	for (int i = 0; i < points.size() / 2; i++) {
		int x = points[i * 2];
		int y = points[i * 2 + 1];

		tryInsertPoint(x, y);
	}

	updateNumPoints(inputPointSet.noPt());
	setUsePDF(true);

	updateGL();
}



void MyPanelOpenGL::clearAll() {
	// Clear all our points, and such data.

	// Clear all the colored polygons.
	bool hasCalculatedColoredPolygons = false;
	renderedPolygons.clear();

	voronoiPolygons_.clear();

	// Clear all the Voronoi computations
	// Reset voronoi components for Fortune's algorithm
	// XXX this is clearly not memory safe
	voronoiVertices_ = new vor::Vertices();

	// Clear all the points.
	inputPointSet.eraseAllPoints();

	// Signals and stuff
	updateNumPoints(inputPointSet.noPt());
	setUsePDF(false);
	setVoronoiComputed(false);

	currentRenderType = IMAGE;

	updateGL();
}



void MyPanelOpenGL::mouseMoveEvent(QMouseEvent *event) {
}



void MyPanelOpenGL::setShowVoronoiSites(bool b) {
	showVoronoiSites = b;

	updateGL();
}



void MyPanelOpenGL::setShowVoronoiEdges(bool b) {
	showVoronoiEdges = b;

	updateGL();
}



void MyPanelOpenGL::setNumPoints1k() {
	numPDFPoints = 1000;
	updateNumPointsToGenerate(1000);
}



void MyPanelOpenGL::setNumPoints5k() {
	numPDFPoints = 5000;
	updateNumPointsToGenerate(5000);
}



void MyPanelOpenGL::setNumPoints(int n) {
	numPDFPoints = n;
	updateNumPointsToGenerate(n);
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

