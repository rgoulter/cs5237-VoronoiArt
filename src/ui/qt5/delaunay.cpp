#include "ui/qt5/delaunay.h"

using std::vector;

using delaunay::PointSetArray;
using delaunay::LongInt;
using delaunay::TriRecord;
using delaunay::createVoronoi;

using namespace ui::qt5;



Delaunay::Delaunay(const PointSetArray<LongInt>& inputPoints) : algorithm_(inputPoints) {
}



const vector<geometry::Polygon>& Delaunay::getVoronoiPolygons() const {
	// XXX read lock on VoroPoly
	// XXX Voronoi Polygons are only set *after* the algorithm has added all the points
	// XXX from the input set to the DirectedGraph.
	// XXX Before this, it's just an empty array..
	// Before the Delaunay Triangulation has finished, this is just an empty vector.
	return algorithm_.getVoronoiPolygons();
}



const PointSetArray<LongInt>& Delaunay::allPoints() const {
	return algorithm_.allPoints();
}



const vector<TriRecord>& Delaunay::getLeafNodes() const{
	// XXX read lock on DirectedGraph
	return leafNodes_;
}



void Delaunay::run() {
	const vector<int>& delaunayPointsToProcess = algorithm_.indicesToProcess();

	// Iterate through the points we need to process.
	for (int pIdx : delaunayPointsToProcess) {
		/// XXX: Write Lock on DirectedGraph

		/// Insert into new Tri into the DAG.
		/// These new triangles mightn't be Locally Delaunay.
		// Return the containing triangle for the point i.
		algorithm_.processPoint(pIdx);

		// TODO: #15: Ideally, could update GL to inform we've updated the UI?
		leafNodes_ = algorithm_.directedGraph().getLeafNodes();

		/// Everything is Locally Delaunay by this point.
	}

	// XXX: #15: Write lock on Voro Polygons
	algorithm_.setVoronoiPolygons(createVoronoi(algorithm_.directedGraph()));

	finished_ = true;
}
