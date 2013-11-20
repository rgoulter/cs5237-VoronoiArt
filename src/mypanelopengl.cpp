#include <QtGui/QMouseEvent>
#include "mypanelopengl.h"
#include <cmath>
#include <QDebug>
#include <QFileDialog>
#include <QString>

#include "SOIL.h"

#include "math.h"
#include <iostream>
#include <fstream>
#include "platform.h"
#include <cstdio>
#include <strstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#include "stopWatch.h"
#include "li.h"
#include "lmath.h"
#include "pointSet.h"

#include "pointSetArray.h"
#include "trist.h"

#include "delaunayTri.h"
#include "directedGraph.h"

#include "delaunay.h"
#include "polypixel.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

void animate(int t);


const int WINDOW_WIDTH_DEFAULT = 1000;
const int WINDOW_HEIGHT_DEFAULT = 700;

int windowWidth = WINDOW_WIDTH_DEFAULT;
int windowHeight = WINDOW_HEIGHT_DEFAULT;


std::vector<int> delaunayPointsToProcess;
PointSetArray inputPointSet; // Add the super triangle stuff to this.
PointSetArray delaunayPointSet; // Add the super triangle stuff to this.
Trist delaunayOldTrist;
Trist delaunayNewTrist;
std::vector<PointSetArray> voronoiEdges; // Data structure to hold voronoi edges.


static StopWatch globalSW;
DirectedGraph dag(delaunayPointSet);
LongInt delta = 5;
LongInt one = 1;

// Variables for Image Logic.
string loadedImageFilename = "";
int loadedImageWidth = -1;
int loadedImageHeight = -1;
unsigned char *loadedImageData;
GLuint loadedImageTexture;

// Variables for colored polygons
bool hasCalculatedColoredPolygons = 0;
vector<ColoredPolygon> renderedPolygons;
string imageName;

// Variables for render state stuff.
bool showVoronoiSites = true;
bool showVoronoiEdges = false;

// Variables for point generation.
int numPDFPoints = 75;

Mat src, src_gray;
Mat dst, dst2, dst3, detected_edges, detected_edges2, detected_edges3;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char* window_name = "Edge Map";
const char* window_name2 = "Edge Map2";
const char* window_name3 = "Edge Map3";

// These three functions are for those who are not familiar with OpenGL, you can change these or even completely ignore them

void drawAPoint (double x,double y) {
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0,0,0);
			glVertex2d(x,y);
		glEnd();
		glPointSize(1);
}

void drawALine (double x1,double y1, double x2, double y2) {
		glPointSize(1);
		glBegin(GL_LINE_LOOP);
		glColor3f(0,0,1);
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
		glEnd();
		glPointSize(1);
}



void drawATriangle (double x1,double y1, double x2, double y2, double x3, double y3) {
		glBegin(GL_POLYGON);
		glColor3f(0,0.5,0);
			glVertex2d(x1,y1);
			glVertex2d(x2,y2);
			glVertex2d(x3,y3);
		glEnd();

}



void drawLoadedTextureImage() {
	if (loadedImageWidth < 0) { return; }

	qDebug("Draw the texture");
	glEnable(GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, loadedImageTexture);

	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (0.0, 0.0, -1.0);

		glTexCoord2f (1.0, 0.0);
		glVertex3f (loadedImageWidth, 0.0, -1.0);

		glTexCoord2f (1.0, 1.0);
		glVertex3f (loadedImageWidth, loadedImageHeight, -1.0);

		glTexCoord2f (0.0, 1.0);
		glVertex3f (0.0, loadedImageHeight, -1.0);
	glEnd ();
	glDisable(GL_TEXTURE_2D);
}



void drawDelaunayStuff() {
	// Draw all DAG leaf triangles.
	vector<TriRecord> leafNodes = dag.getLeafNodes();
	for (int i = 0; i < leafNodes.size(); i++){
		TriRecord leafNode = leafNodes[i];

		int pIndex1 = leafNode.vi_[0];
		int pIndex2 = leafNode.vi_[1];
		int pIndex3 = leafNode.vi_[2];

		// Ignore if from the super triangle (i.e. index too large for input set)
		if(pIndex1 > delaunayPointSet.noPt() - 3 ||
		   pIndex2 > delaunayPointSet.noPt() - 3 || 
		   pIndex3 > delaunayPointSet.noPt() - 3){
			   continue;
		}
		
		// Probably could clean this up..
		LongInt p1x, p1y, p2x, p2y, p3x, p3y;

		delaunayPointSet.getPoint(pIndex1, p1x, p1y);
		delaunayPointSet.getPoint(pIndex2, p2x, p2y);
		delaunayPointSet.getPoint(pIndex3, p3x, p3y);

		
		drawATriangle(p1x.doubleValue(), p1y.doubleValue(),
					  p2x.doubleValue(), p2y.doubleValue(),
					  p3x.doubleValue(), p3y.doubleValue());
		drawALine(p1x.doubleValue(), p1y.doubleValue(),
			      p2x.doubleValue(), p2y.doubleValue());
		drawALine(p2x.doubleValue(), p2y.doubleValue(),
			      p3x.doubleValue(), p3y.doubleValue());
		drawALine(p3x.doubleValue(), p3y.doubleValue(),
			      p1x.doubleValue(), p1y.doubleValue());
	}
}

void drawVoronoiStuff(){
	
	std::vector<PointSetArray>::iterator iter1;
	for (iter1 = voronoiEdges.begin(); iter1 != voronoiEdges.end(); ++iter1)
	{
		PointSetArray polygon = *iter1;
		for (int i = 1; i <= polygon.noPt(); i++)
		{
			int indexval;
			LongInt x1, y1, x2, y2;
			if(i+1 > polygon.noPt()) indexval = 1;
			else indexval = i+1;

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
}



void display (void) {
	drawColoredPolygons();

	if (showVoronoiEdges) {
		drawVoronoiStuff();
	}

	
	if (showVoronoiSites) {
		// Point indices are 1-based here
		// Draw input points
		for (int i = 1; i <= inputPointSet.noPt(); i++){
			LongInt px, py;
			inputPointSet.getPoint(i, px, py);
			drawAPoint(px.doubleValue(), py.doubleValue());
		}
	}
}



void generateColoredPolygons(vector<vector<int>>& polys){
	hasCalculatedColoredPolygons = 0;
	renderedPolygons.clear();

	for (int i = 0; i < polys.size(); i++) {
		// Clip polygon to ensure we have nothing out of bounds
		// vector<int> unclippedPoly = polys[i];
		// vector<int> poly = clipPolygonToRectangle(unclippedPoly, 0, 0, loadedImageWidth, loadedImageHeight);
		vector<int> poly = polys[i];

		int colorIv[3];
		findSomeColor3iv(poly, colorIv);

		ColoredPolygon coloredPoly;

		coloredPoly.poly = poly;
		coloredPoly.rgb[0] = (float) colorIv[0] / 255;
		coloredPoly.rgb[1] = (float) colorIv[1] / 255;
		coloredPoly.rgb[2] = (float) colorIv[2] / 255;

		renderedPolygons.push_back(coloredPoly);
	}

	hasCalculatedColoredPolygons = 1;
}



void generateColoredPolygons(vector<vector<MyPoint>>& myPointPolys){
	// Coerce the PSAs to vec<int> poly representation
	vector<vector<int>> ivPolys;

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



void generateColoredPolygons(vector<PointSetArray>& psas){
	// Coerce the PSAs to vec<int> poly representation
	vector<vector<int>> ivPolys;

	for (int i = 0; i < psas.size(); i++) {
		ivPolys.push_back(coercePSAPolyToIVecPoly(psas[i]));
	}

	generateColoredPolygons(ivPolys);
}



void generateDelaunayColoredPolygons() {
	qDebug("Calculate colored");

	// Pre-condition: 'dag' generated.

	vector<vector<MyPoint>> polys;

	// Create vector of vector of points, from delaunay.
	// Draw all DAG leaf triangles.
	vector<TriRecord> leafNodes = dag.getLeafNodes();
	for (int i = 0; i < leafNodes.size(); i++){
		TriRecord leafNode = leafNodes[i];

		int pIndex1 = leafNode.vi_[0];
		int pIndex2 = leafNode.vi_[1];
		int pIndex3 = leafNode.vi_[2];

		// Ignore if from the super triangle (i.e. index too large for input set)
		if(pIndex1 > delaunayPointSet.noPt() - 3 ||
		   pIndex2 > delaunayPointSet.noPt() - 3 || 
		   pIndex3 > delaunayPointSet.noPt() - 3){
			   continue;
		}
		
		// Probably could clean this up..
		// Since MyPoint is now exposed.
		LongInt p1x, p1y, p2x, p2y, p3x, p3y;

		delaunayPointSet.getPoint(pIndex1, p1x, p1y);
		delaunayPointSet.getPoint(pIndex2, p2x, p2y);
		delaunayPointSet.getPoint(pIndex3, p3x, p3y);

		// Add a polygon for the triangle.
		vector<MyPoint> poly;
		poly.push_back(MyPoint(p1x, p1y));
		poly.push_back(MyPoint(p2x, p2y));
		poly.push_back(MyPoint(p3x, p3y));

		polys.push_back(poly);
	}

	generateColoredPolygons(polys);
}



void refreshProjection() {
	glViewport (0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	// If we haven't loaded an image,
	// we don't particularly care what the coord system is.
	if (loadedImageWidth < 0) {
		// Just some boring thing.
		glOrtho(-1, 1, 1, -1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return;
	}

	double imageRatio = ((double) loadedImageWidth) / loadedImageHeight;
	double windowRatio = ((double) windowWidth) / windowHeight;

	if (imageRatio > windowRatio) {
		double ratio = ((double) windowWidth) / windowHeight;

		int renderWidth = loadedImageWidth;
		int renderHeight = (int) (loadedImageWidth / ratio);

		int delta = (renderHeight - loadedImageHeight) / 2;

		glOrtho(0,
                renderWidth,
                loadedImageHeight + delta,
                -delta,
                -1,
                1);
	} else {
		double ratio = ((double) windowWidth) / windowHeight;
		
		int renderWidth = (int) (loadedImageHeight * ratio);
		int renderHeight = loadedImageHeight;

		int delta = (renderWidth - loadedImageWidth) / 2;

		glOrtho(-delta,
                loadedImageWidth + delta,
                renderHeight,
                0,
                -1,
                1);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



void reshape (int w, int h) {
	windowWidth = w;
	windowHeight = h;

	refreshProjection();
}



void refreshZoom(){
	reshape(WINDOW_WIDTH_DEFAULT, WINDOW_HEIGHT_DEFAULT);
}



void init (void) {
	glClearColor (1.0,1.0,1.0, 1.0);
}



void tryInsertPoint (LongInt x, LongInt y) {
	int ptIndex = inputPointSet.addPoint(x, y);
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


	SOIL_free_image_data(loadedImageData);
	loadedImageData =
		SOIL_load_image(imgFilename.c_str(), &loadedImageWidth, &loadedImageHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D,
		         0,
				 GL_RGB,
				 loadedImageWidth,
				 loadedImageHeight,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 loadedImageData);
	//SOIL_free_image_data(loadedImageData);

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

void MyPanelOpenGL::resizeGL(int width, int height){
    reshape(width, height);
}

void MyPanelOpenGL::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawLoadedTextureImage();
	
	glPushMatrix();
	
	display();

	glPopMatrix();
}


void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {
    //qDebug("Window: %d, %d\n", event->x(), event->y());

	double imageRatio = ((double) loadedImageWidth) / loadedImageHeight;
	double windowRatio = ((double) windowWidth) / windowHeight;

	int renderWidth = 2;
	int renderHeight = 2;

	int deltaX = 1;
	int deltaY = 1;

	if(loadedImageWidth > 0) {
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
	}

	double viewScale = (double) renderWidth / windowWidth;

	int px = (event->x() * viewScale) - deltaX;
	int py = (event->y() * viewScale) - deltaY;

	qDebug("Insert Point: %d, %d\n", px, py);

	tryInsertPoint(px, py);

	updateGL();
}

void MyPanelOpenGL::doDelaunayTriangulation(){
    qDebug("Do Delaunay Triangulation\n");
	tryDelaunayTriangulation();
	//generateDelaunayColoredPolygons(); // too slow.

	updateGL();
}

void MyPanelOpenGL::doVoronoiDiagram(){
    //qDebug("Do Voronoi creation\n");
	voronoiEdges.clear();
	if (delaunayPointSet.myPoints.size() > 0)
		createVoronoi();
	else
	{
		doDelaunayTriangulation();
		createVoronoi();
	}

	// Make the colored polygons from Voronoi.
	generateColoredPolygons(voronoiEdges);

	updateGL();
}

void CannyThreshold(int, void*)
{
    /// Reduce noise with a kernel 3x3
    blur( src_gray, detected_edges, Size(3,3) );

    /// Canny detector
    Canny( detected_edges, detected_edges, 100, 100*ratio, kernel_size );

	GaussianBlur( detected_edges, detected_edges2, Size(7,7), 0, 0);
	//GaussianBlur( detected_edges, detected_edges3, Size(9,9), 0, 0);

    /// Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);
	dst2 = Scalar::all(0);
	dst3 = Scalar::all(0);
    //src_gray.copyTo( dst, detected_edges);
	src_gray.copyTo( dst2, detected_edges2);
	src_gray.copyTo( dst, detected_edges);
	GaussianBlur( dst2, dst3, Size(15,15), 0, 0);
	dst3.convertTo(dst3,-1,2,0);
	subtract( dst3, dst2, dst);

	ofstream fout("output.txt");
	unsigned char *input = (unsigned char*)(dst.data);
	vector<MyPoint> goodPoints;
	for(int i = 0; i < dst.cols; i++){
		for(int j = 0; j < dst.rows; j++){
			fout << (int)input[dst.cols * j + i] << endl;
			if((int)input[dst.cols * j + i] > 150){
				MyPoint point((LongInt)(i), (LongInt)(j));
				goodPoints.push_back(point);
			}
		}
	}
	fout.close();

	srand((unsigned)time(0));
    int random_point;
	int lowest=0, highest=goodPoints.size()-1;
    int range=(highest-lowest)+1;
    for(int index=0; index< numPDFPoints; index++){
        random_point = lowest+int(range*rand()/(RAND_MAX + 1.0));
		tryInsertPoint(goodPoints[random_point].x, goodPoints[random_point].y);
    } 

	//uchar* temp = dst.data;
	//imwrite("C:\upload\edge.jpg",dst);
    imshow( window_name, dst );
	imshow( window_name2, dst2);
	imshow( window_name3, dst3);
}

void generatePDF(string imageName) {
	src = imread(imageName);

	if( !src.data )
    { return; }

	/// Create a matrix of the same type and size as src (for dst)
	dst.create( src.size(), src.type() );

	/// Convert the image to grayscale
	cvtColor( src, src_gray, COLOR_BGR2GRAY );

	/// Create a window
	//namedWindow( window_name, WINDOW_AUTOSIZE );
	//namedWindow( window_name2, WINDOW_AUTOSIZE );
	//namedWindow( window_name3, WINDOW_AUTOSIZE );

	/// Create a Trackbar for user to enter threshold
	//createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
	//createTrackbar( "Min Threshold:", window_name2, &lowThreshold, max_lowThreshold, CannyThreshold );
	//createTrackbar( "Min Threshold:", window_name3, &lowThreshold, max_lowThreshold, CannyThreshold );

	/// Show the image
	CannyThreshold(0, 0);
}

void MyPanelOpenGL::doOpenImage(){
	//get a filename to open
	QString qStr_fileName =
		QFileDialog::getOpenFileName(this,
	                                 tr("Open Image"),
									 ".",
									 tr("Image Files (*.png *.jpg *.bmp)"));
	string filenameStr = qStr_fileName.toStdString();
	imageName = filenameStr;

	qDebug(filenameStr.c_str());

	updateFilename(qStr_fileName); // to Qt textbox
	loadOpenGLTextureFromFilename(filenameStr);
	refreshProjection();
}

void MyPanelOpenGL::doDrawImage(){
	qDebug("Draw OpenGL Image");

	// Set some state so as to draw the image.
	// All drawing must be done from paintGL.

	updateGL();
}

void MyPanelOpenGL::doPDF(){
	generatePDF(imageName);
}

void MyPanelOpenGL::clearAll(){
	// Clear all our points, and such data.

	// Clear all the colored polygons.
	bool hasCalculatedColoredPolygons = false;
	renderedPolygons.clear();

	// Clear all the Voronoi computations
	voronoiEdges.clear();

	// Clear all the points.
    delaunayPointsToProcess.clear();
	inputPointSet.eraseAllPoints();
	delaunayPointSet.eraseAllPoints();
	delaunayOldTrist.eraseAllTriangles();
	delaunayNewTrist.eraseAllTriangles();
	dag.cleardirectedGraph();

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

void MyPanelOpenGL::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}

