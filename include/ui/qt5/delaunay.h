#ifndef UI_QT5_DELAUNAYH
#define UI_QT5_DELAUNAYH

#include <vector>

#include <QRunnable>

#include "delaunay/longint/li.h"

#include "delaunay/delaunay.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#include "geometry/polygon.h"



namespace ui {

namespace qt5 {



class Delaunay : public QRunnable {
public:
	Delaunay(const delaunay::PointSetArray<delaunay::LongInt>& inputPoints);

	virtual void run();

	bool finished() const { return finished_; };

	const std::vector<geometry::Polygon>& getVoronoiPolygons() const;

	const delaunay::PointSetArray<delaunay::LongInt>& allPoints() const;

	const std::vector<delaunay::TriRecord>& getLeafNodes() const;

private:
	// XXX: #15: readWrite locks for DirectedGraph; for VoroPolygons
	delaunay::DelaunayAlgorithm<delaunay::LongInt> algorithm_;
	std::vector<delaunay::TriRecord> leafNodes_;
	bool finished_ = false;
};



}

}

#endif