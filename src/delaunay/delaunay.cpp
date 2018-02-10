#include "delaunay/delaunay.h"

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <iostream>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include "delaunay/directedgraph.h"

#include "tracing.h"
#include "stopwatch.h"

#ifndef NDEBUG
#define DELAUNAY_CHECK
#endif

using std::vector;
using std::cout;
using std::endl;

using geometry::Point;
using geometry::Polygon;



namespace delaunay {

template<typename I>
void runDelaunayTriangulationOn(DirectedGraph<I>& dag) {
	vector<int> delaunayPointsToProcess;

	PointSetArray<I> delaunayPointSet = dag.getPointSet();

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
	int numPoints = delaunayPointSet.noPt(); // DELAUNAY_TRACE_OUTPUT
	int pt = 0;                              // DELAUNAY_TRACE_OUTPUT
	for (int pIdx : delaunayPointsToProcess) {
		TRACE("[runDelaunayTriangulationOn(dag)] adding vertex " << (++pt) << "/" << numPoints);
		/// Insert into new Tri into the DAG.
		/// These new triangles mightn't be Locally Delaunay.
		// Return the containing triangle for the point i.
		dag.addVertex(pIdx);

		/// Everything is Locally Delaunay by this point.
	}
}



// e.g. for point I, tri IJK/JKI/KIJ, finds tri adj. to IJ.
FIndex nextTriangle(int iIdx, const LinkedTriangle& ltriIJK) {
	int edgeIdxIJ, edgeIdxJK, edgeIdxKI;
	ltriIJK.getEdgeIndices(iIdx, edgeIdxIJ, edgeIdxJK, edgeIdxKI);

	return ltriIJK.links_[edgeIdxIJ];
}



template<typename I>
Point<I> pointForTri(const PointSetArray<I>& pointSet, const LinkedTriangle& ltri) {
	int triPIdx1, triPIdx2, triPIdx3;
	ltri.tri_.get(triPIdx1, triPIdx2, triPIdx3);

	// TODO circumCircle may benefit from using TriRecord
	Point<I> circum;
	pointSet.circumCircle(triPIdx1, triPIdx2, triPIdx3, circum);

	return circum;
}



template<typename I>
vector<geometry::Polygon> createVoronoi(const DirectedGraph<I>& dag) {
	vector<geometry::Polygon> voronoiPolygons; // Data structure to hold voronoi edges.

	vector<FIndex> lookupLinkedTri = dag.getLinkedTrianglesLookup();

	const PointSetArray<I>& delaunayPointSet = dag.getPointSet();
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
		const LinkedTriangle& ltri = trist[triIdx];

		// Find delaunay triangles to which this point is linked
		geometry::Polygon polygon;

		// TODO May be convenient if PointSetArray had addPoint(MyPoint)
		const Point<I>& initPt = pointForTri(delaunayPointSet, ltri);
		polygon.addPoint((int) initPt.x.doubleValue(),
		                 (int) initPt.y.doubleValue());

		const FIndex initTriIdx = triIdx;
		triIdx = nextTriangle(dppIdx, ltri);
		while (triIdx != initTriIdx) {
			const LinkedTriangle& ltri = trist[triIdx];
			const Point<I>& voronoiPt = pointForTri(delaunayPointSet, ltri);
			polygon.addPoint((int) voronoiPt.x.doubleValue(),
			                 (int) voronoiPt.y.doubleValue());

			triIdx = nextTriangle(dppIdx, ltri);
		}

		voronoiPolygons.push_back(polygon);
	}

	return voronoiPolygons;
}



template<typename I>
vector<geometry::Polygon> runDelaunayAlgorithm(const PointSetArray<I>& inputPoints) {
	StopWatch voroSW;

	voroSW.reset();
	voroSW.resume();

	DirectedGraph<I> dag(inputPoints);

	cout << "MPOG::doVoronoi, created dag" << endl;

	runDelaunayTriangulationOn(dag);
	//generateDelaunayColoredPolygons(); // too slow.

	voroSW.pause();
	double timeDelaunay = voroSW.ms();
	cout << "TIME: doDelaunayTriangulation() is " << timeDelaunay << endl;
	voroSW.reset();
	voroSW.resume();

	const vector<geometry::Polygon>& voronoiPolygons = createVoronoi(dag); // in `delaunay`

	voroSW.pause();
	double timeCreateVoronoi = voroSW.ms();
	cout << "TIME: createVoronoi() is " << timeCreateVoronoi << endl;
	voroSW.reset();
	voroSW.resume();

	return voronoiPolygons;
}



// Instantiate functions
template void runDelaunayTriangulationOn<LongInt>(DirectedGraph<LongInt>&);
template Point<LongInt> pointForTri<LongInt>(const PointSetArray<LongInt>&, const LinkedTriangle&);
template vector<geometry::Polygon> createVoronoi(const DirectedGraph<LongInt>&);
template vector<geometry::Polygon> runDelaunayAlgorithm(const PointSetArray<LongInt>&);

}


