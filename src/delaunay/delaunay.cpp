#include "delaunay/delaunay.h"

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <iostream>

#include "delaunay/li.h"
#include "delaunay/lmath.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include "delaunay/directedgraph.h"

#ifndef NDEBUG
#define DELAUNAY_CHECK
#endif

using std::vector;
using std::cout;
using std::endl;



namespace delaunay {

/// ???
/// XXX Btw, no need to have dlyPointsToProcess here
void delaunayIterationStep(vector<int>& delaunayPointsToProcess,
                           DirectedGraph& dag) {
	if (delaunayPointsToProcess.size() == 0) {
		return;
	}

	int pIdx = delaunayPointsToProcess[0];
	delaunayPointsToProcess.erase(delaunayPointsToProcess.begin());

	/// Insert into new Tri into the DAG.
	/// These new triangles mightn't be Locally Delaunay.
	// Return the containing triangle for the point i.
	dag.addVertex(pIdx);

	/// Everything is Locally Delaunay by this point.

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



void tryDelaunayTriangulation(DirectedGraph& dag) {
	vector<int> delaunayPointsToProcess;

	PointSetArray delaunayPointSet = dag.getPointSet();

	// Add points 1 ... n - 3 (inclusive) into the set of points to be tested.
	// (0 ... < n - 3 since delaunayPointSet includes bounding triangle at end)
	for (int i = 1; i <= delaunayPointSet.noPt() - 3; i++) {
		delaunayPointsToProcess.push_back(i);
	}

	// Shuffle these points of delaunayPointsToProcess
	srand (time(NULL));
	for (unsigned int i = 0; i < delaunayPointsToProcess.size() / 2; i++) {
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



// e.g. for point I, tri IJK/JKI/KIJ, finds tri adj. to IJ.
FIndex nextTriangle(int iIdx, const LinkedTriangle* ltriIJK) {
	int edgeIdxIJ, edgeIdxJK, edgeIdxKI;
	ltriIJK->getEdgeIndices(iIdx, edgeIdxIJ, edgeIdxJK, edgeIdxKI);

	return ltriIJK->links_[edgeIdxIJ];
}



MyPoint pointForTri(const PointSetArray& pointSet, const LinkedTriangle* ltri) {
	int triPIdx1, triPIdx2, triPIdx3;
	ltri->tri_.get(triPIdx1, triPIdx2, triPIdx3);

	// TODO circumCircle may benefit from using TriRecord
	MyPoint circum;
	pointSet.circumCircle(triPIdx1, triPIdx2, triPIdx3, circum);

	return circum;
}



vector<PointSetArray> createVoronoi(DirectedGraph& dag) {
	vector<PointSetArray> voronoiPolygons; // Data structure to hold voronoi edges.

	vector<FIndex> lookupLinkedTri = dag.getLinkedTrianglesLookup();

	const PointSetArray& delaunayPointSet = dag.getPointSet();
	const Triangulation& trist = dag.getTriangulation();

	// n.b. Voronoi complexes are dual to Delaunay complexes:
	//   In Voronoi: = In Delaunay
	//     Point     =   Triangle
	//     Edge      =   Edge
	//     Polygon   =   Point
	// To construct the Voronoi polygons, we construct a polygon for each point
	// in the Delaunay triangulation (excepting the super-tri vertices).

	for (int dppIdx = 1; dppIdx <= delaunayPointSet.noPt() - 3; dppIdx++) {
		// ltri is *some* triangle which contains the point.
		FIndex triIdx = lookupLinkedTri[dppIdx - 1];
		const LinkedTriangle* ltri = trist[triIdx];

		// Find delaunay triangles to which this point is linked
		// POLYREP:POINTSETARRAY
		PointSetArray polygon;

		// TODO May be convenient if PointSetArray had addPoint(MyPoint)
		const MyPoint& initPt = pointForTri(delaunayPointSet, ltri);
		polygon.addPoint(initPt.x, initPt.y);

		const FIndex initTriIdx = triIdx;
		triIdx = nextTriangle(dppIdx, ltri);
		while (triIdx != initTriIdx) {
			const LinkedTriangle* ltri = trist[triIdx];
			const MyPoint& voronoiPt = pointForTri(delaunayPointSet, ltri);
			polygon.addPoint(voronoiPt.x, voronoiPt.y);

			triIdx = nextTriangle(dppIdx, ltri);
		}

		voronoiPolygons.push_back(polygon);
	}

	return voronoiPolygons;
}

}


