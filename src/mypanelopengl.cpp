#include <QtGui/QMouseEvent>
#include "mypanelopengl.h"
#include <cmath>
#include <QDebug>

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

using namespace std;

void animate(int t);


const int WINDOW_WIDTH_DEFAULT = 1000;
const int WINDOW_HEIGHT_DEFAULT = 700;

int windowWidth = WINDOW_WIDTH_DEFAULT;
int windowHeight = WINDOW_HEIGHT_DEFAULT;


// For "simple" zooming in/out, and "simple" navigation,
//  using just integers should be enough.
// Could be improved, of course.
int viewX = 0;
int viewY = 0;
const int VIEW_SCALE_DEFAULT = 100;
int viewScale = VIEW_SCALE_DEFAULT; // Use integer to scale out of 100.


int delayAmount = 1; // Number of seconds to delay between reading inputs.
std::vector<string> inputLines;
std::vector<int> delaunayPointsToProcess;
PointSetArray inputPointSet; // Add the super triangle stuff to this.
PointSetArray delaunayPointSet; // Add the super triangle stuff to this.
Trist delaunayOldTrist;
Trist delaunayNewTrist;

static StopWatch globalSW;
//PointSetArray myPointSet;
Trist myTrist;
DirectedGraph dag(delaunayPointSet);
LongInt delta = 5;
LongInt one = 1;
int flag = 0; // for knowing if the command 'CD' is called for the first time or not

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



void display (void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(180,1,0,0);
	// draw your output here (erase the following 3 lines)
	/*drawAPoint(100,100);
	drawALine(200,200,300,300);
	drawATriangle(400,400,400,500,500,500);*/

	int i;
	
	// Draw all DAG leaf triangles.
	vector<TriRecord> leafNodes = dag.getLeafNodes();
	for (i = 0; i < leafNodes.size(); i++){
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

	// Point indices are 1-based here
	// Draw input points
	for (i = 1; i <= inputPointSet.noPt(); i++){
		LongInt px, py;
		inputPointSet.getPoint(i, px, py);
		drawAPoint(px.doubleValue(), py.doubleValue());
	}

	glPopMatrix();
}



void reshape (int w, int h) {
	windowWidth = w;
	windowHeight = h;

	int zoomedWidth = (w * viewScale / VIEW_SCALE_DEFAULT);
	int zoomedHeight = (h * viewScale / VIEW_SCALE_DEFAULT);

	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(viewX - zoomedWidth / 2,
			   viewX + zoomedWidth / 2,
			   viewY + zoomedHeight / 2,
			   viewY - zoomedHeight / 2);  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void MyPanelOpenGL::resizeGL(int width, int height){
    reshape(width, height);
}

void MyPanelOpenGL::paintGL(){
    display();
}


void MyPanelOpenGL::mousePressEvent(QMouseEvent *event) {
    //qDebug("%d, %d\n", event->x(), event->y());

	// x, y coordinates are between 0-windowWidth and 0-windowHeight.
	// The window is a view of the world, with the centre of the
	//  window at viewX, viewY; the entire window covers a width of
	//  (w * viewScale / VIEW_SCALE_DEFAULT), similarly for height.

	int xRelToCenter = event->x() - (windowWidth / 2);
	int yRelToCenter = event->y() - (windowHeight / 2);

	int px = (xRelToCenter * viewScale / VIEW_SCALE_DEFAULT) + viewX;
	int py = -((yRelToCenter * viewScale / VIEW_SCALE_DEFAULT) + viewY);

	tryInsertPoint(px, py);

	updateGL();
}

void MyPanelOpenGL::doDelaunayTriangulation(){
    //qDebug("Do Delaunay Triangulation\n");
	tryDelaunayTriangulation();
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