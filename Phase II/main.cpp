#include "math.h"
#include <iostream>
#include <fstream>
#include "GL\glut.h"
#include <windows.h>
#include <cstdio>
#include <fstream>
#include <strstream>
#include <string>
#include <sstream>

#include "stopWatch.h"

// Include the PointSet implementation from the folder we used for
//  Phase I. (Is there a nicer way to do this?)


#include "li.h"
#include "lmath.h"
#include "pointSet.h"

#include "pointSetArray.h"
#include "trist.h"

#include "delaunayTri.h"
#include "directedGraph.h"

using namespace std;


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


int delayAmount = 0; // Number of seconds to delay between reading inputs.
std::vector<string> inputLines;
std::vector<int> delaunayPointsToProcess;
PointSetArray inputPointSet; // Add the super triangle stuff to this.
PointSetArray delaunayPointSet; // Add the super triangle stuff to this.
Trist delaunayOldTrist;
Trist delaunayNewTrist;

static StopWatch globalSW;
PointSetArray myPointSet;
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
	glutSwapBuffers ();
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

	// The below portion is commented out as IP essentially means AP in Phase 3.

	//
	//for (int tri = 0; tri < myTrist.noTri(); tri++) {
	//	int p1Idx, p2Idx, p3Idx;

	//	myTrist.getVertexIdx((OrTri) (tri << 3), p1Idx, p2Idx, p3Idx);

	//	// inTri returns 1 for in, 0 for degenerate, -1 for outside.
	//	// So, the >= or > here is arguable?
	//	// -- Leads to a triangle of colinear points.
	//	if (myPointSet.inTri(p1Idx, p2Idx, p3Idx, ptIndex) > 0) {
	//		// Our new point is in the triangle.
	//		cout << "Insert Point (" << x.printOut() << ", " << y.printOut() << ")" << endl;
	//		cout << "Removing Triangle tId=" << tri << ", pts: " << p1Idx << ", " << p2Idx << ", " << p3Idx << endl;
	//		myTrist.delTri((OrTri) (tri << 3));

	//		int tid = myTrist.makeTri(p1Idx, p2Idx, ptIndex);
	//		cout << "Created Triangle tId=" << tid-1 << ", pts: " << p1Idx << ", " << p2Idx << ", " << ptIndex << endl;
	//		tid = myTrist.makeTri(p2Idx, p3Idx, ptIndex);
	//		cout << "Created Triangle tId=" << tid-1 << ", pts: " << p2Idx << ", " << p3Idx << ", " << ptIndex << endl;
	//		tid = myTrist.makeTri(p3Idx, p1Idx, ptIndex);
	//		cout << "Created Triangle tId=" << tid-1 << ", pts: " << p3Idx << ", " << p1Idx << ", " << ptIndex << endl;

	//		return;
	//	}
	//}
	//myPointSet.deleteLastPoint();
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

/*bool DelaunayTri::isLocallyDelaunay(int pIdx1, int pIdx2){// PointSet pSet is redundant?
	vector<TriRecord> triangles = dag.findNodesforEdge(pIdx1, pIdx2);

	int p1, p2; // indexes of third points of triangles
	for(int i=0; i<triangles.size(); i++){
		for(int j=0; j<3; j++){
			if(i==0 && triangles[i].vi_[j]!=pIdx1 && triangles[i].vi_[j]!=pIdx2)
				p1 = triangles[i].vi_[j];
			if(i==1 && triangles[i].vi_[j]!=pIdx1 && triangles[i].vi_[j]!=pIdx2)
				p2 = triangles[i].vi_[j];
		}
	}

	if(myPointSet.inCircle(p1, pIdx1, pIdx2, p2)>0) return true;
	else return false;
}*/



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



// Call this function when the user pushes the button to do Delaunay Triangulation
void tryDelaunayTriangulation() {
	flag = 1; // Sets the CD enountered flag. Will be reset when the next IP command is encountered

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

	// Iterate through the points we need to process.
	for(int i = 1; i <= delaunayPointSet.noPt()-3; i++){
		int pIdx = i; //delaunayPointsToProcess[i];

		TriRecord tri = dag.findLeafNodeForPoint(pIdx); // Return the containing triangle for the point i.
		dag.addChildrenNodes(pIdx);

		DelaunayTri::legalizeEdge(pIdx, tri.vi_[0], tri.vi_[1]);
		DelaunayTri::legalizeEdge(pIdx, tri.vi_[0], tri.vi_[2]);
		DelaunayTri::legalizeEdge(pIdx, tri.vi_[1], tri.vi_[2]);			

		glutPostRedisplay();
	}
}

void handleInputLine(string line){
	string line_noStr;

	string command;// the command of each line
	string numberStr; // for single LongInt operation
	string outputAns = "Answer of your computation"; // the answer you computed

	

	// Busy-waiting delay between commands.
	ULONGLONG delayStart = globalSW.ms();
	while(globalSW.ms() < delayStart + delayAmount * 1000){
		globalSW.pause();
		globalSW.resume();
	}

	stringstream linestream(line);

	linestream >> line_noStr;
	linestream >> command;         // get the command

	if (!command.compare("IP")) { // it was previously 'AP', but we don't have it anymore
		linestream >> numberStr;
		LongInt p1 = LongInt::LongInt(numberStr.c_str());

		linestream >> numberStr;
		LongInt p2 = LongInt::LongInt(numberStr.c_str());

		// If a delaunay has already been computed, the 3 points on top of the point stack are the delaunay bounding
		// triangle vertices. Remove these before inserting any new points.
		if(flag == 1){
			myPointSet.deleteLastPoint();
			myPointSet.deleteLastPoint();
			myPointSet.deleteLastPoint();
			flag = 0;
		}

		int output = myPointSet.addPoint(p1, p2);

		glutPostRedisplay();

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
	myPointSet.eraseAllPoints();
	myTrist.eraseAllTriangles();

	string line;   // each line of the file

	ifstream inputFile("input.txt",ios::in);


	if (inputFile.fail()) {
		cerr << "Error: Cannot read input file \"" << "input.txt" << "\"";
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



void animate(int t){
	if(inputLines.size() > 0){
		cout << "InputLine: " << inputLines[0] << endl;

		handleInputLine(inputLines[0]);

		inputLines.erase(inputLines.begin());

		glutPostRedisplay();

		glutTimerFunc(1000 * delayAmount, animate, 0);
	}
}



void writeFile () {
	ofstream outfile;
	outfile.open ("savefile.txt", ios::out | ios::trunc);
	int pointCount;
	// Let's add all points;
	// This will include the invalid points we failed to make into triangles.
	// Otherwise, Trist would need to be restructured.
	
	// If flag is set, the last 3 points are for bounding triangle. Avoid this from being written to output file
	// else, we write all points into the file.
	if(flag == 1){
			pointCount = myPointSet.noPt()-3;
		}
	else
	{
		pointCount = myPointSet.noPt();
	}

	for(int pt = 1; pt <= pointCount; pt++){
		LongInt x, y;
		myPointSet.getPoint(pt, x, y); // one-based index
		
		// Don't care about line numbers atm.
		outfile << "0000: IP " << x.printOut() << " " << y.printOut() << endl;
	}

	outfile << "0000: CD"  << endl;

	// The below code has been commented out as it is not needed as part of Phase 3.
	/*for(int tri = 0; tri < myTrist.noTri(); tri++){
		int p1Idx, p2Idx, p3Idx;

		myTrist.getVertexIdx((OrTri) (tri << 3), p1Idx, p2Idx, p3Idx);
		outfile << "0000: OT " << p1Idx << " " << p2Idx << " " << p3Idx << endl;
	}
*/
	outfile.close();
}



void testSomeScratch () {
	// This kindof thing should really be in a GoogleTest or something.

	PointSetArray psa;

	psa.addPoint(3, 4);
	LongInt x = 0;
	LongInt y = 0;
	cout << "Trying: (Should be 1): " << psa.getPoint(0, x, y);
	cout << "Point 0: " << x.printOut() << ", " << y.printOut() << endl;

	cout << "Trying: (Should be 0): " << psa.getPoint(99, x, y);
	cout << "Point 99: " << x.printOut() << ", " << y.printOut() << endl;
}



void keyboard (unsigned char key, int x, int y) {
	switch (key) {
		case 'r':
		case 'R':
			cout << "Reading input file." << endl;
			readFile();
			glutTimerFunc(1000 * delayAmount, animate, 0);
		break;

		case 'S':
		case 's':
			cout << "Writing output file." << endl;
			writeFile();
		break;

		case 't':
			cout << "Test some scratch." << endl;
			testSomeScratch();
		break;

		case 'Q':
		case 'q':
			exit(0);
		break;

		case 'I':
		case 'i':
			// Add zoom scale by 5%
			// MAGIC NUMBER
			viewScale -= 5;
			refreshZoom();
		break;

		case 'O':
		case 'o':
			// Minus zoom scale by 5%
			// MAGIC NUMBER
			viewScale += 5;
			refreshZoom();
		break;

		case 'W':
		case 'w':
			viewY -= 50;
			refreshZoom();
		break;

		case 'X':
		case 'x':
			viewY += 50;
			refreshZoom();
		break;

		case 'A':
		case 'a':
			viewX -= 50;
			refreshZoom();
		break;

		case 'D':
		case 'd':
			viewX += 50;
			refreshZoom();
		break;

		case 'C': //Compute Delaunay for the new set of points.
		case 'c':
			tryDelaunayTriangulation();
		break;

		default:
		break;
	}

	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y) {
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */

	enum {
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};

	if ((button == MOUSE_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		// x, y coordinates are between 0-windowWidth and 0-windowHeight.
		// The window is a view of the world, with the centre of the
		//  window at viewX, viewY; the entire window covers a width of
		//  (w * viewScale / VIEW_SCALE_DEFAULT), similarly for height.

		int xRelToCenter = x - (windowWidth / 2);
		int yRelToCenter = y - (windowHeight / 2);

		int px = (xRelToCenter * viewScale / VIEW_SCALE_DEFAULT) + viewX;
		int py = -((yRelToCenter * viewScale / VIEW_SCALE_DEFAULT) + viewY);

		tryInsertPoint(px, py);
	}

	glutPostRedisplay();
}



int main (int argc, char **argv) {
	cout<<"CS5237 Phase II"<< endl<< endl;

	cout << "Right mouse click: OT operation"<<endl;
	cout << "Q: Quit" <<endl;
	cout << "R: Read in control points from \"input.txt\"" <<endl;
	cout << "S: Write control points to \"savefile.txt\"" <<endl;
	cout << "I: Zoom in" <<endl;
	cout << "O: Zoom out" <<endl;
	cout << "W: Move towards the top of the window" <<endl;
	cout << "X: Move towards the bottom of the window" <<endl;
	cout << "A: Move towards the left of the window" <<endl;
	cout << "D: Move towards the right of the window" <<endl;
	cout << "C: Compute Delaunay triangulation" <<endl;

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (WINDOW_WIDTH_DEFAULT, WINDOW_HEIGHT_DEFAULT);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS5237 Phase II");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}
