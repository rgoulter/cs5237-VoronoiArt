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

#include "polypixel.h"

using namespace std;

void animate(int t);


const int WINDOW_WIDTH_DEFAULT = 1000;
const int WINDOW_HEIGHT_DEFAULT = 700;

int windowWidth = WINDOW_WIDTH_DEFAULT;
int windowHeight = WINDOW_HEIGHT_DEFAULT;


int delayAmount = 1; // Number of seconds to delay between reading inputs.
std::vector<string> inputLines;
std::vector<int> delaunayPointsToProcess;
PointSetArray inputPointSet; // Add the super triangle stuff to this.
PointSetArray delaunayPointSet; // Add the super triangle stuff to this.
Trist delaunayOldTrist;
Trist delaunayNewTrist;
std::vector<PointSetArray> voronoiEdges; // Data structure to hold voronoi edges.

static StopWatch globalSW;
//PointSetArray myPointSet;
Trist myTrist;
DirectedGraph dag(delaunayPointSet);
LongInt delta = 5;
LongInt one = 1;
int flag = 0; // for knowing if the command 'CD' is called for the first time or not

// Variables for Image Logic.
string loadedImageFilename = "";
int loadedImageWidth = -1;
int loadedImageHeight = -1;
unsigned char *loadedImageData;
GLuint loadedImageTexture;

bool hasCalculatedColoredPolygons = 0;
vector<ColoredPolygon> renderedPolygons;



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



void display (void) {
	// draw your output here (erase the following 3 lines)
	/*drawAPoint(100,100);
	drawALine(200,200,300,300);
	drawATriangle(400,400,400,500,500,500);*/

	int i;
	
	// drawDelaunayStuff();

	// Point indices are 1-based here
	// Draw input points
	for (i = 1; i <= inputPointSet.noPt(); i++){
		LongInt px, py;
		inputPointSet.getPoint(i, px, py);
		drawAPoint(px.doubleValue(), py.doubleValue());
	}

	//Test Code
	std::vector<PointSetArray>::iterator it;
	for (it = voronoiEdges.begin(); it != voronoiEdges.end();++it)
	{
		PointSetArray ptSet = *it;
		LongInt ptx1, pty1, ptx2, pty2;
		ptx1 = ptSet.myPoints[0].x;
		pty1 = ptSet.myPoints[0].y;

		ptx2 = ptSet.myPoints[1].x;
		pty2 = ptSet.myPoints[1].y;

		drawALine(ptx1.doubleValue(), pty1.doubleValue(),
			      ptx2.doubleValue(), pty2.doubleValue());


	}
}



void drawColoredPolygons() {
	if (!hasCalculatedColoredPolygons) {
		glPopMatrix();
		return;
	}
	
	glEnable(GL_COLOR_MATERIAL);
	for (int i = 0; i < renderedPolygons.size(); i++) {
		ColoredPolygon coloredPoly = renderedPolygons[i];

		glBegin(GL_POLYGON);
		glColor3fv(coloredPoly.rgb); // rgb?

		for(int j = 0; j < coloredPoly.poly.size(); j++) {
			double x = coloredPoly.poly[j].x.doubleValue();
			double y = coloredPoly.poly[j].y.doubleValue();
			
			glVertex2d(x, y);
		}
		glEnd();
	}
}



void generateColoredPolygons(vector<vector<MyPoint>>& polys){
	hasCalculatedColoredPolygons = 0;
	renderedPolygons.clear();

	for (int i = 0; i < polys.size(); i++) {
		vector<MyPoint> poly = polys[i];

		int colorIv[3];
		findAverageColor3iv(loadedImageTexture, poly, colorIv);

		ColoredPolygon coloredPoly;

		coloredPoly.poly = poly;
		coloredPoly.rgb[0] = (float) colorIv[0] / 255;
		coloredPoly.rgb[1] = (float) colorIv[1] / 255;
		coloredPoly.rgb[2] = (float) colorIv[2] / 255;

		renderedPolygons.push_back(coloredPoly);
	}

	hasCalculatedColoredPolygons = 1;
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


void DelaunayTri::findBoundingTri(PointSetArray &pSet){
	LongInt minX = pSet.myPoints[0].x;
	LongInt maxX = pSet.myPoints[0].x;
	LongInt minY = pSet.myPoints[0].y;
	LongInt maxY = pSet.myPoints[0].y;
	LongInt tempmaxX, tempminX;

	for(int i = 1; i < pSet.myPoints.size(); i++){
		if(minX > pSet.myPoints[i].x) minX = pSet.myPoints[i].x;
		else if(maxX < pSet.myPoints[i].x) maxX = pSet.myPoints[i].x;

		if(minY > pSet.myPoints[i].y) minY = pSet.myPoints[i].y;
		else if(maxY < pSet.myPoints[i].y) maxY = pSet.myPoints[i].y;
	}

	minX = minX-delta;
	tempminX = minX;
	maxX = maxX+delta;
	tempmaxX = maxX;
	minY = minY-delta;
	maxY = maxY+delta;

	pSet.addPoint(maxX+(maxY-minY),minY);
	pSet.addPoint(minX-(maxY-minY),minY);

	while( !(maxX == minX + one || maxX == minX - one || maxX == minX) )
	{
		maxX = maxX - one;
		minX = minX + one ;
	}
	
	pSet.addPoint(maxX,maxY+((tempmaxX-tempminX))); // some rounding may occur if LongInt is odd
	int temp =1;
}



void DelaunayTri::legalizeEdge(int pIdx1, int pIdx2, int pIdx3){
	vector<TriRecord> triangles = dag.findNodesForEdge(pIdx2, pIdx3);

	int p4;
	for(int i=0; i<triangles.size(); i++){
		for(int j=0; j<3; j++){
			if(triangles[i].vi_[j]!=pIdx1 && triangles[i].vi_[j]!=pIdx2 && triangles[i].vi_[j]!=pIdx3){
				p4 = triangles[i].vi_[j];
				if(delaunayPointSet.inCircle(pIdx1, pIdx2, pIdx3, p4)>0){ // Adding 1 to the indexes for the benefit of inCircle method
					dag.addFlipChildrenNodes(pIdx1, pIdx2, pIdx3, p4);
					legalizeEdge(pIdx1, pIdx2, p4);
					legalizeEdge(pIdx1, pIdx3, p4);
				}
				return;
			}
		}
	}
}



void delaunayIterationStep() {
	if(delaunayPointsToProcess.size() == 0){
		return;
	}

	int pIdx = delaunayPointsToProcess[0];
	delaunayPointsToProcess.erase(delaunayPointsToProcess.begin());

	TriRecord tri = dag.findLeafNodeForPoint(pIdx); // Return the containing triangle for the point i.
	dag.addChildrenNodes(pIdx);

	DelaunayTri::legalizeEdge(pIdx, tri.vi_[0], tri.vi_[1]);
	DelaunayTri::legalizeEdge(pIdx, tri.vi_[0], tri.vi_[2]);
	DelaunayTri::legalizeEdge(pIdx, tri.vi_[1], tri.vi_[2]);

	// Redisplay
    //updateGL(); // updateGL is a method of the QGLWidget..
}

// This method checks whether the voronoi edge identified already exists in the existing voronoi edge set.
bool checkedgeExists(PointSetArray voronoiEdge){
	MyPoint dA, dB;
	std::vector<PointSetArray>::iterator iter1;
	for(iter1 = voronoiEdges.begin(); iter1 != voronoiEdges.end();)
	{
		PointSetArray vEdge = *iter1;
		LongInt x1, y1, x2, y2, vx1, vy1, vx2, vy2;
		x1 = vEdge.myPoints[0].x;
		y1 = vEdge.myPoints[0].y;
		x2 = vEdge.myPoints[1].x;
		y2 = vEdge.myPoints[1].y;
		vx1 = voronoiEdge.myPoints[0].x;
		vy1 = voronoiEdge.myPoints[0].y;
		vx2 = voronoiEdge.myPoints[1].x;
		vy2 = voronoiEdge.myPoints[1].y;
		
		if((x1==vx1 && y1==vy1 && x2==vx2 && y2==vy2) || (x1==vx2 && y1==vy2 && x2==vx1 && y2==vy1))
			return true;
		++iter1;
	}

	return false;
}

// This method creates the voronoi edges for a given delaunay triangulation. The voronoi data structure
// consists of a vector of point pairs.
void createVoronoi(){
	
	// Get the current leaf nodes of the DAG, and for each triangle, find the circumcenter and 
	// join it to the circumcenter of the neighbouring triangles.
	vector<TriRecord> dtTriangles = dag.getLeafNodes();
	std::vector<TriRecord>::iterator it;
	
	for (it = dtTriangles.begin(); it != dtTriangles.end();)
	{
		TriRecord tri = *it;
		// FInd neighboring triangle for each edge of the given triangle
		std::vector<TriRecord> tripair1 = dag.findNodesForEdge(tri.vi_[0],tri.vi_[1]);
		std::vector<TriRecord> tripair2 = dag.findNodesForEdge(tri.vi_[1],tri.vi_[2]);
		std::vector<TriRecord> tripair3 = dag.findNodesForEdge(tri.vi_[2],tri.vi_[0]);
		std::vector<TriRecord>::iterator iter1, iter2, iter3;

		// For each of the 3 triangle pairs obtained, get the voronoi edges.
		PointSetArray voronoiEdge1;
		for(iter1 = tripair1.begin(); iter1 != tripair1.end()  && tripair1.size()>1;)
		{
			TriRecord tripair1_element = *iter1;
			MyPoint circum;
			delaunayPointSet.circumCircle(tripair1_element.vi_[0], tripair1_element.vi_[1],tripair1_element.vi_[2], circum);
			voronoiEdge1.addPoint(circum.x,circum.y);
			++iter1;
		}
		
		PointSetArray voronoiEdge2;
		for(iter2 = tripair2.begin(); iter2 != tripair2.end()  && tripair2.size()>1;)
		{
			TriRecord tripair2_element = *iter2;
			MyPoint circum;
			delaunayPointSet.circumCircle(tripair2_element.vi_[0], tripair2_element.vi_[1],tripair2_element.vi_[2], circum);
			voronoiEdge2.addPoint(circum.x,circum.y);
			++iter2;
		}

		PointSetArray voronoiEdge3;
		for(iter3 = tripair3.begin(); iter3 != tripair3.end()  && tripair3.size()>1;)
		{
			TriRecord tripair3_element = *iter3;
			MyPoint circum;
			delaunayPointSet.circumCircle(tripair3_element.vi_[0], tripair3_element.vi_[1],tripair3_element.vi_[2], circum);
			voronoiEdge3.addPoint(circum.x,circum.y);
			++iter3;
		}
		
		// Add voronoi edges to the voronoi data structure, after checking if it already exists.
		if(voronoiEdge1.myPoints.size()!=0 && !checkedgeExists(voronoiEdge1))
		voronoiEdges.push_back(voronoiEdge1);
		if(voronoiEdge2.myPoints.size()!=0 && !checkedgeExists(voronoiEdge2))
		voronoiEdges.push_back(voronoiEdge2);
		if(voronoiEdge3.myPoints.size()!=0 && !checkedgeExists(voronoiEdge3))
		voronoiEdges.push_back(voronoiEdge3);

		++it;
	}
	
	 

}


// Call this function when the user pushes the button to do Delaunay Triangulation
void tryDelaunayTriangulation() {
	//flag = 1; // Sets the CD enountered flag. Will be reset when the next IP command is encountered

	// Erase relevant data structures
	dag.cleardirectedGraph();
	delaunayPointsToProcess.clear();
	delaunayPointSet.eraseAllPoints();
	delaunayOldTrist.eraseAllTriangles();
	delaunayNewTrist.eraseAllTriangles();

	// Copy points from the input set to Delaunay point set
	for(int i = 1; i <= inputPointSet.noPt(); i++){
		LongInt x, y;
		inputPointSet.getPoint(i, x, y);
		delaunayPointSet.addPoint(x, y);
	}

	DelaunayTri::findBoundingTri(delaunayPointSet);
	dag.addChildrenNodes(delaunayPointSet.noPt()); //Tells the DAG what the bounding triangle is, but no inserts into DAG take place here.

	// Add points 1 ... n - 3 (inclusive) into the set of points to be tested.
	// (0 ... < n - 3 since that's what it was)
	// (TODO: Explain: We don't include the last 3 points because...?).
	for(int i = 1; i <= delaunayPointSet.noPt()-3; i++){
		delaunayPointsToProcess.push_back(i);
	}

	// TODO: Shuffle these points of delaunayPointsToProcess
	srand (time(NULL));
	for(int i = 0; i < delaunayPointsToProcess.size() / 2; i++){
		int j = rand() % delaunayPointsToProcess.size();

		// swap
		int tmp = delaunayPointsToProcess[i];
		delaunayPointsToProcess[i] = delaunayPointsToProcess[j];
		delaunayPointsToProcess[j] = tmp;
	}

	// Iterate through the points we need to process.
	// NO ANIMATION, just run each step immediately.
	while(delaunayPointsToProcess.size() > 0) {
		delaunayIterationStep();
	}
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

void handleInputLine(string line){
	string line_noStr;

	string command;// the command of each line
	string numberStr; // for single LongInt operation
	string outputAns = "Answer of your computation"; // the answer you computed

	// Busy-waiting delay between commands.
	StopWatch tmpSW;
	tmpSW.resume();
	while(tmpSW.ms() < delayAmount * 1000){
		tmpSW.pause();
		tmpSW.resume();
	}

	stringstream linestream(line);

	linestream >> line_noStr;
	linestream >> command;         // get the command

	if (!command.compare("IP")) { // it was previously 'AP', but we don't have it anymore
		linestream >> numberStr;
		LongInt p1(numberStr.c_str());

		linestream >> numberStr;
		LongInt p2(numberStr.c_str());

		int output = inputPointSet.addPoint(p1, p2);
		
		// updateGL(); // updateGL() is a method of QGLWidget

		ostringstream convert;
		convert << output;
		outputAns = "#POINT = " + convert.str();

		globalSW.pause();
		cout << line_noStr  << " " << outputAns << " (" << p1.printOut() << ", " << p2.printOut() << ")" << endl;
		globalSW.resume();

	} else if(!command.compare("OT")) {
		int p1Idx, p2Idx, p3Idx;
		linestream >> p1Idx;
		linestream >> p2Idx;			
		linestream >> p3Idx;
			
		int triIdx = myTrist.makeTri(p1Idx, p2Idx, p3Idx, true);
			
		globalSW.pause();
		cout << "Triangle #" << triIdx << " pIdx: " << p1Idx << ", " << p2Idx << ", " << p3Idx << endl;
		globalSW.resume();
	} else if (!command.compare("CD")) {
		tryDelaunayTriangulation();
	} else if (!command.compare("DY")) {
		linestream >> delayAmount;

		cout << "Delay for " << delayAmount << " seconds." << endl;
	} else {
		cerr << "Exception: Wrong input command" << endl;
	}
}



void readFile () {
	inputLines.clear();
	inputPointSet.eraseAllPoints();
	myTrist.eraseAllTriangles();

	string line; // each line of the file

	ifstream inputFile("input.txt",ios::in);


	if (inputFile.fail()) {
		qDebug() << "Error: Cannot read input file \"" << "input.txt" << "\"";
		return;
	}

	while (inputFile.good()) {
		getline(inputFile,line);

		if(line.empty()) {
			continue; 
		} // in case the line has nothing in it

		// Do we need to strcpy?
		cout << "Read in non-empty line: " << line << endl;
		inputLines.push_back(line);
	}

}



void writeFile () {
	ofstream outfile;
	outfile.open ("savefile.txt", ios::out | ios::trunc);
	int pointCount;

	// Let's add all points;
	// This will include the invalid points we failed to make into triangles.
	// Otherwise, Trist would need to be restructured.
	
	pointCount = inputPointSet.noPt();

	for(int pt = 1; pt <= pointCount; pt++){
		LongInt x, y;
		inputPointSet.getPoint(pt, x, y); // one-based index
		
		// Don't care about line numbers atm.
		outfile << "0000: IP " << x.printOut() << " " << y.printOut() << endl;
	}

	outfile << "0000: CD"  << endl;

	outfile.close();
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

	if (hasCalculatedColoredPolygons) {
		qDebug("Draw Colored Polygons");
		drawColoredPolygons();
	}

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
	updateGL();
}

void MyPanelOpenGL::doOpenImage(){
	//get a filename to open
	QString qStr_fileName =
		QFileDialog::getOpenFileName(this,
	                                 tr("Open Image"),
									 ".",
									 tr("Image Files (*.png *.jpg *.bmp)"));
	string filenameStr = qStr_fileName.toStdString();
 
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

void MyPanelOpenGL::mouseMoveEvent(QMouseEvent *event) {
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
