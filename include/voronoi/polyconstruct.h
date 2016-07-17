#ifndef VORONOI_POLYCONSTRUCTH
#define VORONOI_POLYCONSTRUCTH

#include <memory>
#include <unordered_map>
#include <vector>

#include "delaunay/pointsetarray.h"

#include "voronoi/vedge.h"
#include "voronoi/vpoint.h"
#include "voronoi/voronoi.h" // for Edges typedef


namespace voronoi {

/// For use in constructing polygons from edges.
struct HalfWingedEdge {
	HalfWingedEdge(const VPoint *idxA,
	               const VPoint *idxB,
	               double abAngle,
	               double baAngle)
	  : pointIdxA_(idxA), pointIdxB_(idxB),
	    abAngle_(abAngle), baAngle_(baAngle),
	    nextEdge_(nullptr), prevEdge_(nullptr) {
	}

	/// angle *from*..
	double angle(const VPoint *p) const {
		return (p == pointIdxA_) ? abAngle_ : baAngle_;
	}

	bool startsFrom(const VPoint *p) const { return p == pointIdxA_; }

	const VPoint *pointIdxA_;
	const VPoint *pointIdxB_;
	double abAngle_;
	double baAngle_;
	HalfWingedEdge *nextEdge_; // edge b connects to
	HalfWingedEdge *prevEdge_; // edge a connects to
	// POLYREP:POINTSETARRAY
	std::shared_ptr<delaunay::PointSetArray> face_;
};



/// WingedEdge returned won't be linked together.
/// List of WingedEdges won't be sorted, either.
std::unordered_map< const VPoint*, std::vector<HalfWingedEdge *> > buildEdgeLookup(const Edges& edges);



std::vector<HalfWingedEdge*> linkEdges(const Edges& edges);



std::shared_ptr<delaunay::PointSetArray> polygonFromLinkedEdge(HalfWingedEdge *edge);



std::vector<delaunay::PointSetArray> polygonsFromEdges(const Edges& edges);


}

#endif
