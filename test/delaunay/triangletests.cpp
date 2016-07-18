#include <vector>

#include "gtest/gtest.h"

#include "delaunay/polygon.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include "geometry/linesegment.h"

using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(DelaunayTriangleTest, OrientationTest) {
	using geometry::Point;
	using geometry::orientation;

	Point<int> originPt(0, 0);
	Point<int> endPt(100, 0);

	Point<int> ccwPt(50, 50);
	Point<int> colPt(200, 0);
	Point<int> cwPt(50, -50);

	// Test CCW,
	EXPECT_EQ(1, orientation({originPt, endPt}, ccwPt));

	// Test Co-linear
	EXPECT_EQ(0, orientation({originPt, endPt}, colPt));

	// Test CW
	EXPECT_EQ(-1, orientation({originPt, endPt}, cwPt));
}



TEST(DelaunayTriangleTest, OrientationTest2) {
	using geometry::Point;
	using geometry::orientation;

	Point<int> originPt(1000, 1000);
	Point<int>    endPt(1100, 1000);

	Point<int> ccwPt(1050, 1050);
	Point<int> colPt(1200, 1000);
	Point<int>  cwPt(1050,  950);

	// Test CCW,
	EXPECT_EQ(1, orientation({originPt, endPt}, ccwPt));

	// Test Co-linear
	EXPECT_EQ(0, orientation({originPt, endPt}, colPt));

	// Test CW
	EXPECT_EQ(-1, orientation({originPt, endPt}, cwPt));
}



TEST(DelaunayTriangleTest, IntersectsSegSegTest) {
	using geometry::Point;
	using geometry::intersects;

	Point<int> p1(-10, 0);
	Point<int> p2(0, 0);
	Point<int> p3(10, 0);
	Point<int> p4(0, -10);
	Point<int> p5(0, 10);
	Point<int> p6(0, 20);

	EXPECT_EQ(true, intersects({p1, p3}, {p4, p5}));

	// Incident, but not intersecting
	EXPECT_EQ(true, intersects({p1, p3}, {p2, p5}));
	EXPECT_EQ(true, intersects({p1, p3}, {p4, p2}));

	// Completele not touching
	EXPECT_EQ(false, intersects({p1, p3}, {p5, p6}));
}






TEST(DelaunayTriangleTest, IsCCW) {
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

	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW1));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW2));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW3));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW1));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW2));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW3));
}



TEST(DelaunayTriangleTest, IsCCW2) {
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

	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW1));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW2));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW3));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW1));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW2));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW3));
}



TEST(DelaunayTriangleTest, IsCCW3) {
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

	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW1));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW2));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triCCW3));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW1));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW2));
	EXPECT_EQ(false, isTriangleCCW(pointSet, triCW3));
}



TEST(DelaunayTriangleTest, IsCCWFlipTri) {
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

	EXPECT_EQ(true, isTriangleCCW(pointSet, triABD));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triDBC));

	TriRecord triABC(pIdx1, pIdx2, pIdx3);
	TriRecord triACD(pIdx1, pIdx3, pIdx4);

	EXPECT_EQ(true, isTriangleCCW(pointSet, triABC));
	EXPECT_EQ(true, isTriangleCCW(pointSet, triACD));
}



TEST(DelaunayTriangleTest, IntersectsTriSegTest) {
	PointSetArray pointSet;

	// A simple right-angled triangle
	int pIdx1 = pointSet.addPoint(0, 0);
	int pIdx2 = pointSet.addPoint(100, 0);
	int pIdx3 = pointSet.addPoint(0, 100);

	TriRecord tri(pIdx1, pIdx2, pIdx3);

	// hypotenuse of the tri slopes through (50, 50)
	int segPtIdx1 = pointSet.addPoint (40, 40);
	int segPtIdx2 = pointSet.addPoint (60, 60);
	int segPtIdx3 = pointSet.addPoint (100, 100);

	// Test CCW,
	EXPECT_EQ(true, intersectsTriangle(pointSet, tri, segPtIdx1, segPtIdx2));

	// Incident, but not intersecting
	EXPECT_EQ(false, intersectsTriangle(pointSet, tri, segPtIdx2, segPtIdx3));
}



TEST(DelaunayTriangleTest, IntersectsTriTriTest) {
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

	EXPECT_EQ(false, intersectsTriangle(pointSet, triLeft, triRight));


	int pIdx5 = pointSet.addPoint(200, 100);    // right-high

	// intersects triRight
	TriRecord triBad(pIdx2, pIdx4, pIdx5);
	EXPECT_EQ(true,  intersectsTriangle(pointSet, triRight, triBad));
}
