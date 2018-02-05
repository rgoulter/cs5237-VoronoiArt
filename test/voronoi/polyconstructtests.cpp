#include <cmath>

#include <set>
#include <vector>

#include "catch.hpp"

#include "delaunay/pointsetarray.h"
#include "delaunay/polygon.h" // for orientation
#include "delaunay/directedgraph.h"

#include "voronoi/polyconstruct.h"

#include "geometry/polygon.h"

using std::set;
using std::vector;

using delaunay::MyPoint;
using delaunay::PointSetArray;

using geometry::Point;
using geometry::Polygon;
using geometry::orientation;

using namespace voronoi;

#define SUT_NAME "voronoi/PolgonConstruct"
#define SUT_TAGS "[voronoi][PolygonConstruct]"



TEST_CASE(SUT_NAME "/linkEdges, Trivial", SUT_TAGS "[linkEdges]") {
	// VEdge isn't quite pair<Pt,Pt>; needs 'start'
	VPoint *s = new VPoint(0, 0);

	VPoint *a = new VPoint(10, 10);
	VPoint *b = new VPoint(50, 50);
	VPoint *c = new VPoint(90, 10);

	VEdge *ab = new VEdge(s, a, b);
	VEdge *bc = new VEdge(s, b, c);

	Edges edges;
	edges.push_back(ab);
	edges.push_back(bc);

	vector<HalfWingedEdge*> linkedEdges = linkEdges(edges);

	REQUIRE((size_t)4 == linkedEdges.size());

	// Figure out which edge is where
	// (Could prob'ly use a map<> here).
	HalfWingedEdge *linkedEdgeAB;
	HalfWingedEdge *linkedEdgeBA;
	HalfWingedEdge *linkedEdgeBC;
	HalfWingedEdge *linkedEdgeCB;

	for (HalfWingedEdge *linkedEdge : linkedEdges) {
		if (linkedEdge->pointIdxA_ == a &&
		    linkedEdge->pointIdxB_ == b) {
			linkedEdgeAB = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == b &&
		    linkedEdge->pointIdxB_ == a) {
			linkedEdgeBA = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == b &&
		    linkedEdge->pointIdxB_ == c) {
			linkedEdgeBC = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == c &&
		    linkedEdge->pointIdxB_ == b) {
			linkedEdgeCB = linkedEdge;
		}
	}

	REQUIRE_FALSE(linkedEdgeAB == nullptr);
	REQUIRE_FALSE(linkedEdgeBA == nullptr);
	REQUIRE_FALSE(linkedEdgeBC == nullptr);
	REQUIRE_FALSE(linkedEdgeCB == nullptr);

	//
	// assert the angles are as I'd expect them.
	// n.b. atan2 range is from -PI to +PI
	//
	const double angle45 = atan(1);
	REQUIRE( 1 == linkedEdgeAB->abAngle_ / angle45);
	REQUIRE(-3 == linkedEdgeBA->abAngle_ / angle45);

	REQUIRE(-1 == linkedEdgeBC->abAngle_ / angle45);
	REQUIRE( 3 == linkedEdgeCB->abAngle_ / angle45);

	//
	// Now assert that the function linked the right things together
	//

	CHECK(nullptr ==      linkedEdgeAB->prevEdge_);

	CHECK(linkedEdgeBC == linkedEdgeAB->nextEdge_);
	CHECK(linkedEdgeAB == linkedEdgeBC->prevEdge_);

	CHECK(nullptr ==      linkedEdgeBC->nextEdge_);

	CHECK(nullptr ==      linkedEdgeCB->prevEdge_);

	CHECK(linkedEdgeBA == linkedEdgeCB->nextEdge_);
	CHECK(linkedEdgeCB == linkedEdgeBA->prevEdge_);

	CHECK(nullptr ==      linkedEdgeBA->nextEdge_);

	// Clean up...
	delete linkedEdgeAB;
	delete linkedEdgeBA;
	delete linkedEdgeBC;
	delete linkedEdgeCB;
	delete ab;
	delete bc;
	delete a;
	delete b;
	delete c;
}



// Could test polygonFromLinkedEdge as intermediate



TEST_CASE(SUT_NAME "/polygonsFromEdges, Trivial", SUT_TAGS "[polygonsFromEdges]") {
	// VEdge isn't quite pair<Pt,Pt>; needs 'start'
	VPoint *s = new VPoint(0, 0);

	VPoint *a = new VPoint(10, 10);
	VPoint *b = new VPoint(50, 50);
	VPoint *c = new VPoint(90, 10);

	VEdge *ab = new VEdge(s, a, b);
	VEdge *bc = new VEdge(s, b, c);
	VEdge *ca = new VEdge(s, c, a);

	Edges edges;
	edges.push_back(ab);
	edges.push_back(bc);
	edges.push_back(ca);

	// Should return tri<abc> and tri<cba>
	vector<geometry::Polygon> polygons = polygonsFromEdges(edges);
	bool isCCW[2];

	REQUIRE((unsigned) 2 == polygons.size());

	// The following is *really* awkward,
	// partly since atm we don't have good abstractions
	// for dealing with polygons.

	// Need to check that got CCW order.
	for (int polyIdx = 0; (unsigned) polyIdx < polygons.size(); ++polyIdx) {
		const geometry::Polygon& poly = polygons[polyIdx];
		for (unsigned int ptIdx = 0; ptIdx < poly.numPoints(); ++ptIdx) {
			const Point<int>& p1 = poly[ptIdx];

			// Looking for the starting point, (10, 10)
			if (p1.x == 10) {
				// next 0=>1, 1=>2, 2=>0
				const Point<int>& p2 = poly[(ptIdx + 1) % 3];
				// next 0=>2, 1=>0, 2=>1
				const Point<int>& p3 = poly[(ptIdx + 2) % 3];

				int or1 = orientation({p1, p2}, p3);
				int or2 = orientation({p2, p3}, p1);
				int or3 = orientation({p3, p1}, p2);
				CHECK(or1 == or2);
				CHECK(or2 == or3);

				isCCW[polyIdx] = (or1 == 1);

				break;
			}
		}
	}

	// One of them will be true
	CHECK((isCCW[0] || isCCW[1]));
	// Exactly one. (One is CCW, the other isn't).
	CHECK_FALSE(isCCW[0] == isCCW[1]);

	// Clean up...
	delete ab;
	delete bc;
	delete a;
	delete b;
	delete c;
}
