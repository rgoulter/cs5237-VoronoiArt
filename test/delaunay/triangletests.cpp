#include <vector>

#include "catch.hpp"

#include "delaunay/polygon.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include "geometry/linesegment.h"

using std::vector;

using namespace delaunay;

using geometry::Intersection;

#define SUT_NAME "delaunay/TriangleGeometry"
#define SUT_TAGS "[delaunay][TriangleGeometry]"



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW 1", SUT_TAGS "[isTriangleCCW]") {
	PointSetArray pointSet;

	int pIdx1 = pointSet.addPoint(0, 0);
	int pIdx2 = pointSet.addPoint(100, 0);
	int pIdx3 = pointSet.addPoint(50, 50);

	TriRecord triCCW1(pIdx1, pIdx2, pIdx3);
	TriRecord triCCW2(pIdx2, pIdx3, pIdx1);
	TriRecord triCCW3(pIdx3, pIdx1, pIdx2);
	TriRecord triCW1(pIdx3, pIdx2, pIdx1);
	TriRecord triCW2(pIdx2, pIdx1, pIdx3);
	TriRecord triCW3(pIdx1, pIdx3, pIdx2);

	CHECK(isTriangleCCW(pointSet, triCCW1));
	CHECK(isTriangleCCW(pointSet, triCCW2));
	CHECK(isTriangleCCW(pointSet, triCCW3));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW1));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW2));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW3));
}



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW 2", SUT_TAGS "[isTriangleCCW]") {
	PointSetArray pointSet;

	int pIdx1 = pointSet.addPoint(-100, -100);
	int pIdx2 = pointSet.addPoint(100, 0);
	int pIdx3 = pointSet.addPoint(-250, 50);

	TriRecord triCCW1(pIdx1, pIdx2, pIdx3);
	TriRecord triCCW2(pIdx2, pIdx3, pIdx1);
	TriRecord triCCW3(pIdx3, pIdx1, pIdx2);
	TriRecord triCW1(pIdx3, pIdx2, pIdx1);
	TriRecord triCW2(pIdx2, pIdx1, pIdx3);
	TriRecord triCW3(pIdx1, pIdx3, pIdx2);

	CHECK(isTriangleCCW(pointSet, triCCW1));
	CHECK(isTriangleCCW(pointSet, triCCW2));
	CHECK(isTriangleCCW(pointSet, triCCW3));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW1));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW2));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW3));
}



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW 3", SUT_TAGS "[isTriangelCCW]") {
	PointSetArray pointSet;

	int pIdx1 = pointSet.addPoint(1000, 1000);
	int pIdx2 = pointSet.addPoint(1100, 1000);
	int pIdx3 = pointSet.addPoint(1050, 1050);

	TriRecord triCCW1(pIdx1, pIdx2, pIdx3);
	TriRecord triCCW2(pIdx2, pIdx3, pIdx1);
	TriRecord triCCW3(pIdx3, pIdx1, pIdx2);
	TriRecord triCW1(pIdx3, pIdx2, pIdx1);
	TriRecord triCW2(pIdx2, pIdx1, pIdx3);
	TriRecord triCW3(pIdx1, pIdx3, pIdx2);

	CHECK(isTriangleCCW(pointSet, triCCW1));
	CHECK(isTriangleCCW(pointSet, triCCW2));
	CHECK(isTriangleCCW(pointSet, triCCW3));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW1));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW2));
	CHECK_FALSE(isTriangleCCW(pointSet, triCW3));
}



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW Flipped Triangles", SUT_TAGS "[isTriangleCCW]") {
	PointSetArray pointSet;

	// abd, dbc are CCW.
	// so,
	// abc, acd should be also
	int pIdx1 = pointSet.addPoint(425, 342);
	int pIdx2 = pointSet.addPoint(396, 483);
	int pIdx3 = pointSet.addPoint(120, 31);
	int pIdx4 = pointSet.addPoint(306, 57);

	TriRecord triABD(pIdx1, pIdx2, pIdx4);
	TriRecord triDBC(pIdx4, pIdx2, pIdx3);

	CHECK(isTriangleCCW(pointSet, triABD));
	CHECK(isTriangleCCW(pointSet, triDBC));

	TriRecord triABC(pIdx1, pIdx2, pIdx3);
	TriRecord triACD(pIdx1, pIdx3, pIdx4);

	CHECK(isTriangleCCW(pointSet, triABC));
	CHECK(isTriangleCCW(pointSet, triACD));
}



TEST_CASE(SUT_NAME "/intersectsTriangle, Segment Test", SUT_TAGS "[intersectsTriangle]") {
	PointSetArray pointSet;

	// A simple right-angled triangle
	int pIdx1 = pointSet.addPoint(0, 0);
	int pIdx2 = pointSet.addPoint(100, 0);
	int pIdx3 = pointSet.addPoint(0, 100);

	TriRecord tri(pIdx1, pIdx2, pIdx3);

	// hypotenuse of the tri slopes through (50, 50)
	int segPtIdx1 = pointSet.addPoint (40, 40);
	int segPtIdx2 = pointSet.addPoint (50, 50);
	int segPtIdx3 = pointSet.addPoint (60, 60);
	int segPtIdx4 = pointSet.addPoint (100, 100);

	// Test CCW,
	CHECK(Intersection::Overlap == intersectsTriangle(pointSet, tri, {segPtIdx1, segPtIdx3}));

	// Incidental, but not overlapping
	CHECK(Intersection::Incidental == intersectsTriangle(pointSet, tri, {segPtIdx2, segPtIdx4}));
	CHECK(Intersection::None == intersectsTriangle(pointSet, tri, {segPtIdx3, segPtIdx4}));
}



TEST_CASE(SUT_NAME "/intersectsTriangle, Triangle Test", SUT_TAGS "[intersectsTriangle]") {
	PointSetArray pointSet;

	// Two simple right-angled triangles
	int pIdx1 = pointSet.addPoint(0, 0);      // left
	int pIdx2 = pointSet.addPoint(100, 0);    // mid-low
	int pIdx3 = pointSet.addPoint(100, 100);  // mid-high
	int pIdx4 = pointSet.addPoint(200, 0);    // right

	// these two touch (adjacent along p2p3),
	// but don't overlap
	TriRecord triLeft(pIdx1, pIdx2, pIdx3);
	TriRecord triRight(pIdx2, pIdx4, pIdx3);

	CHECK(Intersection::Incidental == intersectsTriangle(pointSet, triLeft, triRight));


	int pIdx5 = pointSet.addPoint(200, 100);    // right-high

	// intersects triRight
	TriRecord triBad(pIdx2, pIdx4, pIdx5);
	CHECK(Intersection::Overlap ==  intersectsTriangle(pointSet, triRight, triBad));

	// TODO also test tris which don't touch
}
