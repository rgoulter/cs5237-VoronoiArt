#include "ui/qt5/delaunay.h"

#include <QReadLocker>
#include <QWriteLocker>

using std::vector;

using delaunay::PointSetArray;
using delaunay::LongInt;
using delaunay::TriRecord;
using delaunay::createVoronoi;

using namespace ui::qt5;



Delaunay::Delaunay(const PointSetArray<LongInt>& inputPoints) : algorithm_(inputPoints) {
}



const vector<geometry::Polygon>& Delaunay::getVoronoiPolygons() {
	QReadLocker lockUntilCanRead(&voronoiPolygonsLock_);

	// Before the Delaunay Triangulation has finished, this is just an empty vector.
	return algorithm_.getVoronoiPolygons();
}



const PointSetArray<LongInt>& Delaunay::allPoints() const {
	return algorithm_.allPoints();
}



const vector<TriRecord>& Delaunay::getLeafNodes() {
	QReadLocker lockUntilCanRead(&leafNodesLock_);

	return leafNodes_;
}



void Delaunay::run() {
	const vector<int>& delaunayPointsToProcess = algorithm_.indicesToProcess();

	// Iterate through the points we need to process.
	for (int pIdx : delaunayPointsToProcess) {
		/// Insert into new Tri into the DAG.
		/// These new triangles mightn't be Locally Delaunay.
		// Return the containing triangle for the point i.
		QReadLocker lockUntilCanRead(&leafNodesLock_);
		algorithm_.processPoint(pIdx);

		// TODO: #15: Ideally, could update GL to inform we've updated the UI?
		{
			QWriteLocker lockUntilCanWrite(&leafNodesLock_);
			leafNodes_ = algorithm_.directedGraph().getLeafNodes();
		}

		/// Everything is Locally Delaunay by this point.
	}

	{
		QWriteLocker lockUntilCanWrite(&voronoiPolygonsLock_);
		algorithm_.setVoronoiPolygons(createVoronoi(algorithm_.directedGraph()));
	}

	finished_ = true;
}
