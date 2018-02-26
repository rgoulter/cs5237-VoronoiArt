#include "ui/qt5/delaunay.h"

#include <QReadLocker>
#include <QWriteLocker>

using std::vector;

using delaunay::PointSetArray;
using delaunay::LongInt;
using delaunay::TriRecord;
using delaunay::createVoronoi;

using namespace ui::qt5;



Delaunay::Delaunay(const PointSetArray<LongInt>& inputPoints, QObject *parent) : QObject(parent), algorithm_(inputPoints) {
	setAutoDelete(false);
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

	int numProcessed = 0;
	int total = delaunayPointsToProcess.size();

	// Iterate through the points we need to process.
	for (int pIdx : delaunayPointsToProcess) {
		/// Insert into new Tri into the DAG.
		/// These new triangles mightn't be Locally Delaunay.
		// Return the containing triangle for the point i.
		QReadLocker lockUntilCanRead(&leafNodesLock_);
		algorithm_.processPoint(pIdx);

		{
			QWriteLocker lockUntilCanWrite(&leafNodesLock_);
			leafNodes_ = algorithm_.directedGraph().getLeafNodes();
		}

		numProcessed++;
		emit progressed(numProcessed, total + 1);

		/// Everything is Locally Delaunay by this point.
	}

	{
		QWriteLocker lockUntilCanWrite(&voronoiPolygonsLock_);
		algorithm_.setVoronoiPolygons(createVoronoi(algorithm_.directedGraph()));
		emit progressed(total + 1, total + 1);
	}

	finished_ = true;
}
