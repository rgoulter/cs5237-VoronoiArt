#include "delaunay.h"

#include <stdlib.h>
#include <time.h>

#include "li.h"
#include "lmath.h"
#include "pointSet.h"

#include "pointSetArray.h"
#include "trist.h"

#include "delaunayTri.h"
#include "directedGraph.h"

using std::vector;



extern vector<int> delaunayPointsToProcess;
extern PointSetArray inputPointSet; // Add the super triangle stuff to this.
extern PointSetArray delaunayPointSet; // Add the super triangle stuff to this.
extern Trist delaunayOldTrist;
extern Trist delaunayNewTrist;
extern vector<PointSetArray> voronoiEdges; // Data structure to hold voronoi edges.


extern DirectedGraph dag;
extern LongInt delta;
extern LongInt one;


void DelaunayTri::findBoundingTri(PointSetArray &pSet) {
	LongInt minX = pSet.myPoints[0].x;
	LongInt maxX = pSet.myPoints[0].x;
	LongInt minY = pSet.myPoints[0].y;
	LongInt maxY = pSet.myPoints[0].y;
	LongInt tempmaxX, tempminX, thousand = 2000;

	for (int i = 1; i < pSet.myPoints.size(); i++) {
		if (minX > pSet.myPoints[i].x)
			minX = pSet.myPoints[i].x;
		else if (maxX < pSet.myPoints[i].x)
			maxX = pSet.myPoints[i].x;

		if (minY > pSet.myPoints[i].y) minY = pSet.myPoints[i].y;
		else if (maxY < pSet.myPoints[i].y) maxY = pSet.myPoints[i].y;
	}

	minX = minX-delta-thousand;
	//tempminX = minX;
	maxX = maxX+delta+thousand;
	//tempmaxX = maxX;
	minY = minY-delta-thousand;
	maxY = maxY+delta+thousand;

	pSet.addPoint(maxX+(maxY-minY),minY);
	pSet.addPoint(minX-(maxY-minY),minY);

	maxX = (maxX.doubleValue() - minX.doubleValue()) / 2;

	pSet.addPoint((LongInt) ((maxX.doubleValue() + minX.doubleValue()) / 2),
	              maxY+((maxX-minX))); // some rounding may occur if LongInt is odd

	int temp = 1;
}



void DelaunayTri::legalizeEdge(int pIdx1, int pIdx2, int pIdx3) {
	vector<TriRecord> triangles = dag.findNodesForEdge(pIdx2, pIdx3);

	int p4;
	for (int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (triangles[i].vi_[j] != pIdx1 &&
			    triangles[i].vi_[j] != pIdx2 &&
			    triangles[i].vi_[j] != pIdx3) {
				p4 = triangles[i].vi_[j];

				// Adding 1 to the indexes for the benefit of inCircle method
				if (delaunayPointSet.inCircle(pIdx1, pIdx2, pIdx3, p4) > 0) {
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
	if (delaunayPointsToProcess.size() == 0) {
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
bool checkedgeExists(PointSetArray voronoiEdge) {
	MyPoint dA, dB;
	std::vector<PointSetArray>::iterator iter1;
	for (iter1 = voronoiEdges.begin(); iter1 != voronoiEdges.end();)
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

		if ((x1==vx1 && y1==vy1 && x2==vx2 && y2==vy2) ||
		    (x1==vx2 && y1==vy2 && x2==vx1 && y2==vy1))
			return true;
		++iter1;
	}

	return false;
}





void createVoronoi() {
	for (int dppIdx = 1; dppIdx <= delaunayPointSet.noPt() - 3; dppIdx++)
	{

		// Find delaunay triangles to which this point is linked
		std::vector<TriRecord> linkedTriangles = dag.findlinkedNodes(dppIdx);
		PointSetArray polygon;

		// findlinkedNodes method gives an ordered list of triangles. Iterate through and find circumcenters.
		std::vector<TriRecord>::iterator iter1;
		for (iter1 = linkedTriangles.begin(); iter1 != linkedTriangles.end();)
		{
			TriRecord tri = *iter1;
			MyPoint circum;
			delaunayPointSet.circumCircle(tri.vi_[0], tri.vi_[1],tri.vi_[2], circum);
			polygon.addPoint(circum.x,circum.y);
			++iter1;
		}

		voronoiEdges.push_back(polygon);
	}
}




// Call this function when the user pushes the button to do Delaunay Triangulation
void tryDelaunayTriangulation() {
	// Erase relevant data structures
	dag.cleardirectedGraph();
	delaunayPointsToProcess.clear();
	delaunayPointSet.eraseAllPoints();
	delaunayOldTrist.eraseAllTriangles();
	delaunayNewTrist.eraseAllTriangles();

	// Copy points from the input set to Delaunay point set
	for (int i = 1; i <= inputPointSet.noPt(); i++) {
		LongInt x, y;
		inputPointSet.getPoint(i, x, y);
		delaunayPointSet.addPoint(x, y);
	}

	DelaunayTri::findBoundingTri(delaunayPointSet);
	dag.addChildrenNodes(delaunayPointSet.noPt()); //Tells the DAG what the bounding triangle is, but no inserts into DAG take place here.

	// Add points 1 ... n - 3 (inclusive) into the set of points to be tested.
	// (0 ... < n - 3 since that's what it was)
	// (TODO: Explain: We don't include the last 3 points because...?).
	for (int i = 1; i <= delaunayPointSet.noPt() - 3; i++) {
		delaunayPointsToProcess.push_back(i);
	}

	// TODO: Shuffle these points of delaunayPointsToProcess
	srand (time(NULL));
	for (int i = 0; i < delaunayPointsToProcess.size() / 2; i++) {
		int j = rand() % delaunayPointsToProcess.size();

		// swap
		int tmp = delaunayPointsToProcess[i];
		delaunayPointsToProcess[i] = delaunayPointsToProcess[j];
		delaunayPointsToProcess[j] = tmp;
	}

	// Iterate through the points we need to process.
	// NO ANIMATION, just run each step immediately.
	while (delaunayPointsToProcess.size() > 0) {
		delaunayIterationStep();
	}
}
