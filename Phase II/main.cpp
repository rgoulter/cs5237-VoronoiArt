
#include "..\basicsP2\pointSetArray.h"

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


static StopWatch globalSW;
PointSetArray myPointSet;
Trist myTrist;



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

	// draw your output here (erase the following 3 lines)
	/*drawAPoint(100,100);
	drawALine(200,200,300,300);
	drawATriangle(400,400,400,500,500,500);*/

	int i;
	for (i = 0; i < myTrist.noTri(); i++) {
		// We want to be able to iterate through triangles.
		int pIndex1, pIndex2, pIndex3;
		
		myTrist.getVertexIdx((OrTri) (i << 3), pIndex1, pIndex2, pIndex3);

		// Probably could clean this up..
		LongInt p1x, p1y, p2x, p2y, p3x, p3y;

		myPointSet.getPoint(pIndex1, p1x, p1y);
		myPointSet.getPoint(pIndex2, p2x, p2y);
		myPointSet.getPoint(pIndex3, p3x, p3y);

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
	for (i = 1; i <= myPointSet.noPt(); i++){
		LongInt px, py;
		myPointSet.getPoint(i, px, py);
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
	int ptIndex = myPointSet.addPoint(x, y);
	
	for (int tri = 0; tri < myTrist.noTri(); tri++) {
		int p1Idx, p2Idx, p3Idx;

		myTrist.getVertexIdx((OrTri) (tri << 3), p1Idx, p2Idx, p3Idx);

		// inTri returns 1 for in, 0 for degenerate, -1 for outside.
		// So, the >= or > here is arguable?
		// -- Leads to a triangle of colinear points.
		if (myPointSet.inTri(p1Idx, p2Idx, p3Idx, ptIndex) >= 0) {
			// Our new point is in the triangle.
			cout << "Insert Point (" << x.printOut() << ", " << y.printOut() << ")" << endl;
			cout << "Removing Triangle tId=" << tri << ", pts: " << p1Idx << ", " << p2Idx << ", " << p3Idx << endl;
			myTrist.delTri((OrTri) (tri << 3));

			myTrist.makeTri(p1Idx, p2Idx, ptIndex);
			myTrist.makeTri(p2Idx, p3Idx, ptIndex);
			myTrist.makeTri(p3Idx, p1Idx, ptIndex);

			return;
		}
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

	if (!command.compare("AP")) {
		linestream >> numberStr;
		LongInt p1 = LongInt::LongInt(numberStr.c_str());

		linestream >> numberStr;
		LongInt p2 = LongInt::LongInt(numberStr.c_str());

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

	} else if(!command.compare("IP")){
		linestream >> numberStr;
		LongInt p1 = LongInt::LongInt(numberStr.c_str());

		linestream >> numberStr;
		LongInt p2 = LongInt::LongInt(numberStr.c_str());

		tryInsertPoint(p1, p2);

		globalSW.pause();
		globalSW.resume();
		
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

	// Let's add all points;
	// This will include the invalid points we failed to make into triangles.
	// Otherwise, Trist would need to be restructured.
	for(int pt = 1; pt <= myPointSet.noPt(); pt++){
		LongInt x, y;
		myPointSet.getPoint(pt, x, y); // one-based index
		
		// Don't care about line numbers atm.
		outfile << "0000: AP " << x.printOut() << " " << y.printOut() << endl;
	}

	for(int tri = 0; tri < myTrist.noTri(); tri++){
		int p1Idx, p2Idx, p3Idx;

		myTrist.getVertexIdx((OrTri) (tri << 3), p1Idx, p2Idx, p3Idx);
		outfile << "0000: OT " << p1Idx << " " << p2Idx << " " << p3Idx << endl;
	}

	outfile.close();
}



void testSomeScratchShit () {
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

		case 'w':
			cout << "Writing output file." << endl;
			writeFile();
		break;

		case 't':
			cout << "Test some scratch shit." << endl;
			testSomeScratchShit();
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
			viewY -= 50;
			refreshZoom();
		break;

		case 'S':
			viewY += 50;
			refreshZoom();
		break;

		case 'A':
			viewX -= 50;
			refreshZoom();
		break;

		case 'D':
			viewX += 50;
			refreshZoom();
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
		int py = (yRelToCenter * viewScale / VIEW_SCALE_DEFAULT) + viewY;

		tryInsertPoint(px, py);
	}

	glutPostRedisplay();
}



int main (int argc, char **argv) {
	cout<<"CS5237 Phase II"<< endl<< endl;

	cout << "Right mouse click: OT operation"<<endl;
	cout << "Q: Quit" <<endl;
	cout << "R: Read in control points from \"input.txt\"" <<endl;
	cout << "W: Write control points to \"input.txt\"" <<endl;

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
