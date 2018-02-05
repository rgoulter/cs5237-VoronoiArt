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

	SECTION("CCW Triangle 123") {
		TriRecord triCCW(pIdx1, pIdx2, pIdx3);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangles 231") {
		TriRecord triCCW(pIdx2, pIdx3, pIdx1);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangles 312") {
		TriRecord triCCW(pIdx3, pIdx1, pIdx2);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}

	SECTION("CW Triangle 321") {
		TriRecord triCW(pIdx3, pIdx2, pIdx1);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 213") {
		TriRecord triCW(pIdx2, pIdx1, pIdx3);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 132") {
		TriRecord triCW(pIdx1, pIdx3, pIdx2);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
}



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW 2", SUT_TAGS "[isTriangleCCW]") {
	PointSetArray pointSet;

	int pIdx1 = pointSet.addPoint(-100, -100);
	int pIdx2 = pointSet.addPoint(100, 0);
	int pIdx3 = pointSet.addPoint(-250, 50);

	SECTION("CCW Triangle 123") {
		TriRecord triCCW(pIdx1, pIdx2, pIdx3);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangle 231") {
		TriRecord triCCW(pIdx2, pIdx3, pIdx1);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangle 312") {
		TriRecord triCCW(pIdx3, pIdx1, pIdx2);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}

	SECTION("CW Triangle 321") {
		TriRecord triCW(pIdx3, pIdx2, pIdx1);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 213") {
		TriRecord triCW(pIdx2, pIdx1, pIdx3);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 132") {
		TriRecord triCW(pIdx1, pIdx3, pIdx2);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
}



TEST_CASE(SUT_NAME "/isTriangleCCW, IsCCW 3", SUT_TAGS "[isTriangelCCW]") {
	PointSetArray pointSet;

	int pIdx1 = pointSet.addPoint(1000, 1000);
	int pIdx2 = pointSet.addPoint(1100, 1000);
	int pIdx3 = pointSet.addPoint(1050, 1050);

	SECTION("CCW Triangle 123") {
		TriRecord triCCW(pIdx1, pIdx2, pIdx3);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangle 231") {
		TriRecord triCCW(pIdx2, pIdx3, pIdx1);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}
	SECTION("CCW Triangle 312") {
		TriRecord triCCW(pIdx3, pIdx1, pIdx2);
		CHECK(isTriangleCCW(pointSet, triCCW));
	}

	SECTION("CW Triangle 321") {
		TriRecord triCW(pIdx3, pIdx2, pIdx1);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 213") {
		TriRecord triCW(pIdx2, pIdx1, pIdx3);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
	SECTION("CW Triangle 132") {
		TriRecord triCW(pIdx1, pIdx3, pIdx2);
		CHECK_FALSE(isTriangleCCW(pointSet, triCW));
	}
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

	SECTION("Triangles 124, 423") {
		TriRecord triABD(pIdx1, pIdx2, pIdx4);
		TriRecord triDBC(pIdx4, pIdx2, pIdx3);

		CHECK(isTriangleCCW(pointSet, triABD));
		CHECK(isTriangleCCW(pointSet, triDBC));
	}

	SECTION("Flipped Triangles 123, 134") {
		TriRecord triABC(pIdx1, pIdx2, pIdx3);
		TriRecord triACD(pIdx1, pIdx3, pIdx4);

		CHECK(isTriangleCCW(pointSet, triABC));
		CHECK(isTriangleCCW(pointSet, triACD));
	}
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

	SECTION("Segment s1-s3 overlaps with Tri 123") {
		// Test CCW,
		CHECK(Intersection::Overlap == intersectsTriangle(pointSet, tri, {segPtIdx1, segPtIdx3}));
	}

	SECTION("Segment s2-s4 is incidental to Tri 123") {
		// Incidental, but not overlapping
		CHECK(Intersection::Incidental == intersectsTriangle(pointSet, tri, {segPtIdx2, segPtIdx4}));
	}

	SECTION("Segment s3-s4 doesn't intersect with Tri 123") {
		CHECK(Intersection::None == intersectsTriangle(pointSet, tri, {segPtIdx3, segPtIdx4}));
	}
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

	SECTION("Triangle 123 is incidental to Triangle 243") {
		CHECK(Intersection::Incidental == intersectsTriangle(pointSet, triLeft, triRight));
	}


	SECTION("Triangle 245 overlaps Triangle 243") {
		int pIdx5 = pointSet.addPoint(200, 100);    // right-high

		// intersects triRight
		TriRecord triBad(pIdx2, pIdx4, pIdx5);
		CHECK(Intersection::Overlap ==  intersectsTriangle(pointSet, triRight, triBad));
	}

	// TODO also test tris which don't touch
}
