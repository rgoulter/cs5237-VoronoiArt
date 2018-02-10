#include <vector>

#include "catch.hpp"

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"

using std::vector;

using namespace delaunay;

#define SUT_NAME "delaunay/PointSetArray"
#define SUT_TAGS "[delaunay][PointSetArray]"



TEST_CASE(SUT_NAME "/inTri<LongInt>, Simple", SUT_TAGS "[LongInt]") {
	PointSetArray<LongInt> ps;

	int p1 = ps.addPoint(0, 0);   // 1
	int p2 = ps.addPoint(0, 4);   // 2
	int p3 = ps.addPoint(4, 0);   // 3
	int p4 = ps.addPoint(4, 4);   // 4
	int p5 = ps.addPoint(1, 1);   // 5

	SECTION("Point within Triangle") {
		// (1, 1) IS in tri 123
		CHECK(1 == ps.inTri(p1, p2, p3, p5));
	}

	SECTION("Point on the triangle") {
		// (1, 1) IS in tri 124
		CHECK(0 == ps.inTri(p1, p2, p4, p5));
	}

	SECTION("Point is not in the triangle") {
		// (1, 1) IS NOT in tri 234
		REQUIRE(-1 == ps.inTri(p2, p3, p4, p5));
	}
}
TEST_CASE(SUT_NAME "/inTri<double>, Simple", SUT_TAGS "[double]") {
	PointSetArray<double> ps;

	int p1 = ps.addPoint(0, 0);   // 1
	int p2 = ps.addPoint(0, 4);   // 2
	int p3 = ps.addPoint(4, 0);   // 3
	int p4 = ps.addPoint(4, 4);   // 4
	int p5 = ps.addPoint(1, 1);   // 5

	SECTION("Point within Triangle") {
		// (1, 1) IS in tri 123
		CHECK(1 == ps.inTri(p1, p2, p3, p5));
	}

	SECTION("Point on the triangle") {
		// (1, 1) IS in tri 124
		CHECK(0 == ps.inTri(p1, p2, p4, p5));
	}

	SECTION("Point is not in the triangle") {
		// (1, 1) IS NOT in tri 234
		REQUIRE(-1 == ps.inTri(p2, p3, p4, p5));
	}
}



TEST_CASE(SUT_NAME "/inCircle<LongInt>, Simple Cases", SUT_TAGS "[LongInt]") {
	PointSetArray<LongInt> pointSet;

	int p1 = pointSet.addPoint(0,   0);
	int p2 = pointSet.addPoint(100, 0);
	int p3 = pointSet.addPoint(100, 100);
	int p4 = pointSet.addPoint(0, 100);

	int p5 = pointSet.addPoint(50, 50);
	int p6 = pointSet.addPoint(2000, 2000);

	SECTION("Trivially Within") {
		// (50,50) is *within* the circum-circle of <p1,p2,p4>
		REQUIRE(1 == pointSet.inCircle(p1, p2, p4, p5));
	}

	SECTION("Trivially On") {
		// (100,100) is *on* the circum-circle of <p1,p2,p4>
		REQUIRE(0 == pointSet.inCircle(p1, p2, p4, p3));
	}

	SECTION("Trivial Outside") {
		// (200,200) is *outside* the circum-circle of <p1,p2,p4>
		REQUIRE(-1 == pointSet.inCircle(p1, p2, p4, p6));
	}
}
TEST_CASE(SUT_NAME "/inCircle<double>, Simple Cases", SUT_TAGS "[double]") {
	PointSetArray<double> pointSet;

	int p1 = pointSet.addPoint(0,   0);
	int p2 = pointSet.addPoint(100, 0);
	int p3 = pointSet.addPoint(100, 100);
	int p4 = pointSet.addPoint(0, 100);

	int p5 = pointSet.addPoint(50, 50);
	int p6 = pointSet.addPoint(2000, 2000);

	SECTION("Trivially Within") {
		// (50,50) is *within* the circum-circle of <p1,p2,p4>
		REQUIRE(1 == pointSet.inCircle(p1, p2, p4, p5));
	}

	SECTION("Trivially On") {
		// (100,100) is *on* the circum-circle of <p1,p2,p4>
		REQUIRE(0 == pointSet.inCircle(p1, p2, p4, p3));
	}

	SECTION("Trivial Outside") {
		// (200,200) is *outside* the circum-circle of <p1,p2,p4>
		REQUIRE(-1 == pointSet.inCircle(p1, p2, p4, p6));
	}
}



// Delaunay algorithm breaking, e.g. of invalid:
//  1. (374,112)
//  2. (-6514,-2005)
//  3. (128,6768)
//  4. (147,376)
TEST_CASE(SUT_NAME "/inCircle<LongInt>, Outside Delaunay Legal", SUT_TAGS "[LongInt]") {
	PointSetArray<LongInt> pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(374, 112);
	int p2 = pointSet.addPoint(-6514, -2005);
	int p3 = pointSet.addPoint(128, 6768);
	int p4 = pointSet.addPoint(147, 376);

	// Both triangles legal.
	CHECK(-1 == pointSet.inCircle(p1, p2, p4, p3));
	REQUIRE(-1 == pointSet.inCircle(p4, p2, p3, p1));
}
TEST_CASE(SUT_NAME "/inCircle<double>, Outside Delaunay Legal", SUT_TAGS "[double]") {
	PointSetArray<double> pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(374, 112);
	int p2 = pointSet.addPoint(-6514, -2005);
	int p3 = pointSet.addPoint(128, 6768);
	int p4 = pointSet.addPoint(147, 376);

	// Both triangles legal.
	CHECK(-1 == pointSet.inCircle(p1, p2, p4, p3));
	REQUIRE(-1 == pointSet.inCircle(p4, p2, p3, p1));
}


// Delaunay algorithm breaking, e.g. of invalid:
//  1. (425,342)
//  2. (306,57)
//  3. (120,31)
//  4. (396,483)
//  *both* abd, dbc are illegal
TEST_CASE(SUT_NAME "/inCircle<LongInt>, Outside Delaunay Illegal", SUT_TAGS "[LongInt]") {
	PointSetArray<LongInt> pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(425, 342);
	int p2 = pointSet.addPoint(306, 57);
	int p3 = pointSet.addPoint(120, 31);
	int p4 = pointSet.addPoint(396, 483);

	// Both triangles illegal. (i.e. contains point)
	CHECK(1 == pointSet.inCircle(p1, p2, p4, p3));
	REQUIRE(1 == pointSet.inCircle(p4, p2, p3, p1));
}
TEST_CASE(SUT_NAME "/inCircle<int>, Outside Delaunay Illegal", SUT_TAGS "[double]") {
	PointSetArray<double> pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(425, 342);
	int p2 = pointSet.addPoint(306, 57);
	int p3 = pointSet.addPoint(120, 31);
	int p4 = pointSet.addPoint(396, 483);

	// Both triangles illegal. (i.e. contains point)
	CHECK(1 == pointSet.inCircle(p1, p2, p4, p3));
	REQUIRE(1 == pointSet.inCircle(p4, p2, p3, p1));
}
