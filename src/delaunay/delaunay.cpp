#include "delaunay.h"

#include <stdlib.h>
#include <time.h>

#include "li.h"
#include "lmath.h"
#include "pointset.h"

#include "pointsetarray.h"
#include "trist.h"

#include "delaunaytri.h"
#include "directedgraph.h"

using std::vector;



// Adds the points to the PointSetArray
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

		if (minY > pSet.myPoints[i].y)
		    minY = pSet.myPoints[i].y;
		else if (maxY < pSet.myPoints[i].y)
		    maxY = pSet.myPoints[i].y;
	}

	LongInt delta = 5; // used in delaunay.cpp

	minX = minX - delta - thousand;
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



void DelaunayTri::legalizeEdge(DirectedGraph& dag, int pIdx1, int pIdx2, int pIdx3) {
	vector<TriRecord> triangles = dag.findNodesForEdge(pIdx2, pIdx3);

	for (int i = 0; i < triangles.size(); i++) {
		for (int j = 0; j < 3; j++) {
			int pointIdx = triangles[i].pointIndexOf(j);

			if (pointIdx != pIdx1 && pointIdx != pIdx2 && pointIdx != pIdx3) {
				int p4 = pointIdx;

				// Presumably delaunayPointSet === dag.getPointSet()
				// so this is legit
				PointSetArray pointSet = dag.getPointSet();

				if (pointSet.inCircle(pIdx1, pIdx2, pIdx3, p4) > 0) {
					dag.addFlipChildrenNodes(pIdx1, pIdx2, pIdx3, p4);
					legalizeEdge(dag, pIdx1, pIdx2, p4);
					legalizeEdge(dag, pIdx1, pIdx3, p4);
				}

				return;
			}
		}
	}
}



void delaunayIterationStep(vector<int>& delaunayPointsToProcess,
                           DirectedGraph& dag) {
	if (delaunayPointsToProcess.size() == 0) {
		return;
	}

	int pIdx = delaunayPointsToProcess[0];
	delaunayPointsToProcess.erase(delaunayPointsToProcess.begin());

	TriRecord tri = dag.findLeafNodeForPoint(pIdx); // Return the containing triangle for the point i.
	dag.addChildrenNodes(pIdx);

	int triPIdx1, triPIdx2, triPIdx3;
	tri.get(triPIdx1, triPIdx2, triPIdx3);

	DelaunayTri::legalizeEdge(dag, pIdx, triPIdx1, triPIdx2);
	DelaunayTri::legalizeEdge(dag, pIdx, triPIdx1, triPIdx3);
	DelaunayTri::legalizeEdge(dag, pIdx, triPIdx2, triPIdx3);

	// Redisplay
	// updateGL(); // updateGL is a method of the QGLWidget..
}



// This method checks whether the voronoi edge identified already exists in the existing voronoi edge set.
// bool checkedgeExists(PointSetArray voronoiEdge) {
// 	MyPoint dA, dB;
// 	std::vector<PointSetArray>::iterator iter1;
// 	for (iter1 = voronoiEdges.begin(); iter1 != voronoiEdges.end();) {
// 		PointSetArray vEdge = *iter1;
// 		LongInt x1, y1, x2, y2, vx1, vy1, vx2, vy2;
// 		x1 = vEdge.myPoints[0].x;
// 		y1 = vEdge.myPoints[0].y;
// 		x2 = vEdge.myPoints[1].x;
// 		y2 = vEdge.myPoints[1].y;
// 		vx1 = voronoiEdge.myPoints[0].x;
// 		vy1 = voronoiEdge.myPoints[0].y;
// 		vx2 = voronoiEdge.myPoints[1].x;
// 		vy2 = voronoiEdge.myPoints[1].y;
//
// 		if ((x1==vx1 && y1==vy1 && x2==vx2 && y2==vy2) ||
// 		    (x1==vx2 && y1==vy2 && x2==vx1 && y2==vy1))
// 			return true;
// 		++iter1;
// 	}
//
// 	return false;
// }



DirectedGraph dagFromInputPoints(const PointSetArray& inputPointSet) {
	PointSetArray delaunayPointSet;

	// Copy points from the input set to Delaunay point set
	for (int i = 1; i <= inputPointSet.noPt(); i++) {
		LongInt x, y;
		inputPointSet.getPoint(i, x, y);
		delaunayPointSet.addPoint(x, y);
	}

	// Add a triangle which bounds all the points.
	DelaunayTri::findBoundingTri(delaunayPointSet);

	DirectedGraph dag(delaunayPointSet);

	// TODO I've no clue what this does / why it's here.
	dag.addChildrenNodes(delaunayPointSet.noPt());

	return dag;
}



void tryDelaunayTriangulation(DirectedGraph& dag) {
	vector<int> delaunayPointsToProcess;

	PointSetArray delaunayPointSet = dag.getPointSet();

	// Add points 1 ... n - 3 (inclusive) into the set of points to be tested.
	// (0 ... < n - 3 since delaunayPointSet includes bounding triangle at end)
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
		delaunayIterationStep(delaunayPointsToProcess, dag);
	}
}



vector<PointSetArray> createVoronoi(DirectedGraph& dag) {
	vector<PointSetArray> voronoiEdges; // Data structure to hold voronoi edges.

	PointSetArray delaunayPointSet = dag.getPointSet();

	for (int dppIdx = 1; dppIdx <= delaunayPointSet.noPt() - 3; dppIdx++) {
		// Find delaunay triangles to which this point is linked
		std::vector<TriRecord> linkedTriangles = dag.findLinkedNodes(dppIdx);
		PointSetArray polygon;

		// findlinkedNodes method gives an ordered list of triangles. Iterate through and find circumcenters.
		std::vector<TriRecord>::iterator iter1;
		for (iter1 = linkedTriangles.begin(); iter1 != linkedTriangles.end();) {
			TriRecord tri = *iter1;
			MyPoint circum;
            std::cout << "circumCircle" << std::endl;
			// TODO circumCircle may benefit from using TriRecord
			int triPIdx1, triPIdx2, triPIdx3;
			tri.get(triPIdx1, triPIdx2, triPIdx3);
			delaunayPointSet.circumCircle(triPIdx1, triPIdx2,triPIdx3, circum);
			polygon.addPoint(circum.x,circum.y);
			++iter1;
		}

		voronoiEdges.push_back(polygon);
	}

	return voronoiEdges;
}

