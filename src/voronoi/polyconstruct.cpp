#include "voronoi/polyconstruct.h"

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <list>

using std::list;
using std::unordered_map;
using std::vector;

using delaunay::PointSetArray;


namespace voronoi {




/// WingedEdge returned won't be linked together.
/// List of WingedEdges won't be sorted, either.
unordered_map< const VPoint*, vector<HalfWingedEdge *> > buildEdgeLookup(const Edges& edges) {
	using std::find;
	using std::make_pair;

	unordered_map< const VPoint*, vector<HalfWingedEdge *> > edgeMap;

	for (Edges::const_iterator iter = edges.begin(); iter != edges.end(); ++iter) {
		const VEdge& edge = *(*iter);
		const VPoint *leftPt  = edge.left;
		const VPoint *rightPt = edge.right;

		// ASSUMES, for the given structure, for any
		//   VPoint *p, *q,
		//   *p == *q => p == q

		double leftRAngle = atan2(rightPt->y - leftPt->y,
		                          rightPt->x - leftPt->x);
		double rightLAngle = atan2(leftPt->y - rightPt->y,
		                           leftPt->x - rightPt->x);

		// n.b. unordered_map uses default c'tor in case where key not present
		vector<HalfWingedEdge *>& leftEdges = edgeMap[leftPt];
		HalfWingedEdge *leftHWE =
			new HalfWingedEdge(leftPt, rightPt, leftRAngle, rightLAngle);
		leftEdges.push_back(leftHWE);

		vector<HalfWingedEdge *>& rightEdges = edgeMap[rightPt];
		HalfWingedEdge *rightHWE =
			new HalfWingedEdge(rightPt, leftPt, rightLAngle, leftRAngle);
		rightEdges.push_back(rightHWE);
	}

	return edgeMap;
}



/// WingedEdge is linked already
vector<HalfWingedEdge*> linkEdges(const Edges& edges) {
	typedef unordered_map< const VPoint*, vector<HalfWingedEdge*> > EdgeMap;
	const EdgeMap& edgeMap = buildEdgeLookup(edges);

	vector<HalfWingedEdge*> allEdges;

	// Iterate through the point/vec<HWE>
	for (EdgeMap::const_iterator iter = edgeMap.begin();
	     iter != edgeMap.end();
	     ++iter) {
		const EdgeMap::value_type& item = *iter;
		const VPoint *pt = item.first;
		vector<HalfWingedEdge*> edges = item.second;  // copy

		// Sort edges by angle,
		// but if the angle (from pt -> other pt) is the same,
		// pick the each which comes-into pt (before the edge which
		// goes-from pt).
		sort(edges.begin(),
		     edges.end(),
		     [pt](const HalfWingedEdge* e1, const HalfWingedEdge* e2)
		{ // a comes before b?
			double e1Angle = e1->angle(pt);
			double e2Angle = e2->angle(pt);
			if (e1Angle < e2Angle) {
				return true;
			} else if (e1Angle == e2Angle) {
				return !e1->startsFrom(pt);
			} else {
				return false;
			}
		});

		// Since the above sorts *by edge*,
		// we need to adjust order slighly:
		edges.push_back(edges.front());
		edges.erase(edges.begin());

		// Now link the edges
		// (Since it's sorted 
		for (vector<HalfWingedEdge*>::iterator sortedEdgeIter = edges.begin();
		     sortedEdgeIter != edges.end();
		     sortedEdgeIter += 2) {
			HalfWingedEdge *outgoingEdge = *(sortedEdgeIter);
			HalfWingedEdge *incomingEdge = *(sortedEdgeIter + 1);

			incomingEdge->nextEdge_ = outgoingEdge;
			outgoingEdge->prevEdge_ = incomingEdge;

			// Push only the outgoing edge.
			// (Edges incoming to pt are outgoing from another pt).
			allEdges.push_back(outgoingEdge);
		}
	}

	return allEdges;
}



// POLY polyFromEdge(WingedEdge wedge) {
	// might return unbounded,

	// if not closed
	// might have to backtrack and construct clockwise..
// }
// POLYREP:POINTSETARRAY
PointSetArray* polygonFromLinkedEdge(HalfWingedEdge *edge) {
	// Need *pointer* to polygon, so that edges know whether
	// they've been 'visited'.
	PointSetArray *poly = new PointSetArray();

	list<const VPoint*> polyPoints;

	HalfWingedEdge *startingEdge = edge;
	const VPoint *startingPt = startingEdge->pointIdxA_;

	// walk CCW
	polyPoints.push_back(edge->pointIdxA_);
	edge = edge->nextEdge_;
	while (edge != nullptr &&
	       edge->pointIdxA_ != startingPt) {
		polyPoints.push_back(edge->pointIdxA_);
		edge->face_ = poly;
		edge = edge->nextEdge_;
	}

	// ran out of edges walking CCW without closing the polygon,
	// so go CW from the start.
	if (edge == nullptr) {
		edge = startingEdge->prevEdge_;

		// Since every time we assigned a next/prev edge,
		// the corresponding link was assigned;
		// and we ran into a not-set edge in CCW direction,
		// must run into a not-set edge in CW direction.
		while (edge != nullptr) {
			polyPoints.push_front(edge->pointIdxA_);
			edge->face_ = poly;
			edge = startingEdge->prevEdge_;
		}
	}

	// Coerce the vector of VPoint* to a POLYREP:PointSetArray
	for (const VPoint* pt : polyPoints) {
		poly->addPoint((int) pt->x, (int) pt->y);
	}

	return poly;
}



// vec<POLY> polygonsFromEdges(Edges *edges) {
//   iter through list<WingedEdge>,
//   shouldn't need to use each edge more than 2x
// }

}


