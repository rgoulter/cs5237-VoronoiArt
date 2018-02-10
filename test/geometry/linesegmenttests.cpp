#include <vector>

#include "catch.hpp"

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include "geometry/linesegment.h"

using std::vector;

using delaunay::LongInt;

using namespace geometry;

#define SUT_NAME "geometry/LineSegment"
#define SUT_TAGS "[geometry][LineSegment]"



TEST_CASE(SUT_NAME "/orientation<LongInt>, Trivial", SUT_TAGS "[orientation][LongInt]") {
	using geometry::Point;
	using geometry::orientation;

	Point<LongInt> originPt(0, 0);
	Point<LongInt> endPt(100, 0);

	SECTION("Point oriented CCW of segment") {
		Point<LongInt> ccwPt(50, 50);
		CHECK(1 == orientation({originPt, endPt}, ccwPt));
	}

	SECTION("Point oriented co-linear with segment") {
		Point<LongInt> colPt(200, 0);
		CHECK(0 == orientation({originPt, endPt}, colPt));
	}

	SECTION("Point oriented CW of segment") {
		Point<LongInt> cwPt(50, -50);
		CHECK(-1 == orientation({originPt, endPt}, cwPt));
	}
}
TEST_CASE(SUT_NAME "/orientation<int>, Trivial", SUT_TAGS "[orientation][int]") {
	using geometry::Point;
	using geometry::orientation;

	Point<int> originPt(0, 0);
	Point<int> endPt(100, 0);

	SECTION("Point oriented CCW of segment") {
		Point<int> ccwPt(50, 50);
		CHECK(1 == orientation({originPt, endPt}, ccwPt));
	}

	SECTION("Point oriented co-linear with segment") {
		Point<int> colPt(200, 0);
		CHECK(0 == orientation({originPt, endPt}, colPt));
	}

	SECTION("Point oriented CW of segment") {
		Point<int> cwPt(50, -50);
		CHECK(-1 == orientation({originPt, endPt}, cwPt));
	}
}



TEST_CASE(SUT_NAME "/orientation<LongInt>, Trivial 2", SUT_TAGS "[orientation][LongInt]") {
	using geometry::Point;
	using geometry::orientation;

	Point<LongInt> originPt(1000, 1000);
	Point<LongInt>    endPt(1100, 1000);

	SECTION("Point oriented CCW of segment") {
		Point<LongInt> ccwPt(1050, 1050);
		CHECK(1 == orientation({originPt, endPt}, ccwPt));
	}

	SECTION("Point oriented co-linear wtih segment") {
		Point<LongInt> colPt(1200, 1000);
		CHECK(0 == orientation({originPt, endPt}, colPt));
	}

	SECTION("Point oriented CW of segment") {
		Point<LongInt>  cwPt(1050,  950);
		CHECK(-1 == orientation({originPt, endPt}, cwPt));
	}
}
TEST_CASE(SUT_NAME "/orientation<int>, Trivial 2", SUT_TAGS "[orientation][int]") {
	using geometry::Point;
	using geometry::orientation;

	Point<int> originPt(1000, 1000);
	Point<int>    endPt(1100, 1000);

	SECTION("Point oriented CCW of segment") {
		Point<int> ccwPt(1050, 1050);
		CHECK(1 == orientation({originPt, endPt}, ccwPt));
	}

	SECTION("Point oriented co-linear wtih segment") {
		Point<int> colPt(1200, 1000);
		CHECK(0 == orientation({originPt, endPt}, colPt));
	}

	SECTION("Point oriented CW of segment") {
		Point<int>  cwPt(1050,  950);
		CHECK(-1 == orientation({originPt, endPt}, cwPt));
	}
}



TEST_CASE(SUT_NAME "/intersects, Segment-Segment", SUT_TAGS "[intersects]") {
	using geometry::Point;
	using geometry::intersects;
	using geometry::Intersection;
	using geometry::isOverlapping;

	Point<int> p1(-10, 0);
	Point<int> p2(0, 0);
	Point<int> p3(10, 0);
	Point<int> p4(0, -10);
	Point<int> p5(0, 10);
	Point<int> p6(0, 20);

	// The compiler doesn't like brace-enclosed initialiser lists to intersects,
	// so be explicit

	SECTION("Overlapping Segments") {
		CHECK(Intersection::Overlap == intersects<int>({p1, p3}, {p4, p5}));
	}

	SECTION("Incident, but not intersecting") {
		CHECK(Intersection::Incidental == intersects<int>({p1, p3}, {p2, p5}));
		CHECK(Intersection::Incidental == intersects<int>({p1, p3}, {p4, p2}));
	}

	SECTION("Completely not touching") {
		CHECK(Intersection::None == intersects<int>({p1, p3}, {p5, p6}));
	}
}



TEST_CASE(SUT_NAME "/intersects, Segment-Segment Colinear Overlapping", SUT_TAGS "[intersects]") {
	using geometry::Point;
	using geometry::intersects;
	using geometry::Intersection;
	using geometry::isOverlapping;

	Point<int> p1(  0, 0);
	Point<int> p2( 10, 0);
	Point<int> p3( 20, 0);
	Point<int> p4( 30, 0);

	// The compiler doesn't like brace-enclosed initialiser lists to intersects,
	// so be explicit
	SECTION("Incidental, but not intersecting") {
		CHECK(Intersection::Incidental == intersects<int>({p1, p2}, {p2, p3}));
	}

	SECTION("Co-linear Overlap") {
		CHECK(Intersection::ColinearOverlap == intersects<int>({p1, p3}, {p2, p4}));
	}

	SECTION("No intersection") {
		CHECK(Intersection::None == intersects<int>({p1, p2}, {p3, p4}));
	}
}



TEST_CASE(SUT_NAME "/findIntersectionPoint, Basic", SUT_TAGS "[findIntersectionPoint]") {
	Point<int> a = {  0,  0 };
	Point<int> b = { 10, 10 };
	Point<int> c = {  0,  2 };
	Point<int> d = {  2,  0 };

	// Should be this
	Point<int> expectedIP(1,1);

	Point<int> actualIP =
		findIntersectionPoint({ a, b }, { c, d });

	CHECK(expectedIP == actualIP);
}
