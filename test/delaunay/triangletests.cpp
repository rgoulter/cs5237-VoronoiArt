#include <vector>

#include "gtest/gtest.h"

#include "polygon.h"
#include "pointsetarray.h"
#include "trist.h"

using std::vector;
using std::cout;
using std::endl;



TEST(DelaunayTriangleTest, OrientationTest) {
	PointSetArray pointSet;

	MyPoint originPt(0, 0);
	MyPoint endPt(100, 0);

	MyPoint ccwPt(50, 50);
	MyPoint colPt(200, 0);
	MyPoint cwPt(50, -50);

	// Test CCW,
	EXPECT_EQ(1, orientation(originPt, endPt, ccwPt));

	// Test Co-linear
	EXPECT_EQ(0, orientation(originPt, endPt, colPt));

	// Test CW
	EXPECT_EQ(-1, orientation(originPt, endPt, cwPt));
}



TEST(DelaunayTriangleTest, OrientationTest2) {
	PointSetArray pointSet;

	MyPoint originPt(1000, 1000);
	MyPoint    endPt(1100, 1000);

	MyPoint ccwPt(1050, 1050);
	MyPoint colPt(1200, 1000);
	MyPoint  cwPt(1050,  950);

	// Test CCW,
	EXPECT_EQ(1, orientation(originPt, endPt, ccwPt));

	// Test Co-linear
	EXPECT_EQ(0, orientation(originPt, endPt, colPt));

	// Test CW
	EXPECT_EQ(-1, orientation(originPt, endPt, cwPt));
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

