#include <vector>

#include "gtest/gtest.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/directedgraph.h"

using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(PointSetTest, PointSetInTriSimple) {
	PointSetArray ps;

	int p1 = ps.addPoint(0, 0);   // 1
	int p2 = ps.addPoint(0, 4);   // 2
	int p3 = ps.addPoint(4, 0);   // 3
	int p4 = ps.addPoint(4, 4);   // 4
	int p5 = ps.addPoint(1, 1);   // 5

	// (1, 1) IS in tri 123
	EXPECT_EQ(1, ps.inTri(p1, p2, p3, p5));

	// (1, 1) IS in tri 124
	EXPECT_EQ(0, ps.inTri(p1, p2, p4, p5));

	// (1, 1) IS NOT in tri 234
	EXPECT_EQ(-1, ps.inTri(p2, p3, p4, p5));
}



TEST(PointSetTest, InCircleTrivialWithin) {
	PointSetArray pointSet;

	int p1 = pointSet.addPoint(0,   0);
	int p2 = pointSet.addPoint(100, 0);
	// int p3 = pointSet.addPoint(100, 100);
	int p4 = pointSet.addPoint(0, 100);

	int p5 = pointSet.addPoint(50, 50);
	// int p6 = pointSet.addPoint(2000, 2000);

	// (50,50) is *within* the circum-circle of <p1,p2,p4>
	EXPECT_EQ(1, pointSet.inCircle(p1, p2, p4, p5));
}



TEST(PointSetTest, InCircleTrivialOn) {
	PointSetArray pointSet;

	int p1 = pointSet.addPoint(0,   0);
	int p2 = pointSet.addPoint(100, 0);
	int p3 = pointSet.addPoint(100, 100);
	int p4 = pointSet.addPoint(0, 100);

	// int p5 = pointSet.addPoint(50, 50);
	// int p6 = pointSet.addPoint(2000, 2000);

	// (100,100) is *on* the circum-circle of <p1,p2,p4>
	EXPECT_EQ(0, pointSet.inCircle(p1, p2, p4, p3));
}



TEST(PointSetTest, InCircleTrivialOutside) {
	PointSetArray pointSet;

	int p1 = pointSet.addPoint(0,   0);
	int p2 = pointSet.addPoint(100, 0);
	// int p3 = pointSet.addPoint(100, 100);
	int p4 = pointSet.addPoint(0, 100);

	// int p5 = pointSet.addPoint(50, 50);
	int p6 = pointSet.addPoint(2000, 2000);

	// (200,200) is *outside* the circum-circle of <p1,p2,p4>
	EXPECT_EQ(-1, pointSet.inCircle(p1, p2, p4, p6));
}



// Delaunay algorithm breaking, e.g. of invalid:
//  1. (374,112)
//  2. (-6514,-2005)
//  3. (128,6768)
//  4. (147,376)
TEST(PointSetTest, InCircleOutsideDelaunayLegal) {
	PointSetArray pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(374, 112);
	int p2 = pointSet.addPoint(-6514, -2005);
	int p3 = pointSet.addPoint(128, 6768);
	int p4 = pointSet.addPoint(147, 376);

	// Both triangles legal.
	EXPECT_EQ(-1, pointSet.inCircle(p1, p2, p4, p3));
	EXPECT_EQ(-1, pointSet.inCircle(p4, p2, p3, p1));
}


// Delaunay algorithm breaking, e.g. of invalid:
//  1. (425,342)
//  2. (306,57)
//  3. (120,31)
//  4. (396,483)
//  *both* abd, dbc are illegal
TEST(PointSetTest, InCircleOutsideDelaunayIllegal) {
	PointSetArray pointSet;

	// abd is a tri, as is dbc.
	// abd doesn't contain c,
	// dbc doesn't contain a
	int p1 = pointSet.addPoint(425, 342);
	int p2 = pointSet.addPoint(306, 57);
	int p3 = pointSet.addPoint(120, 31);
	int p4 = pointSet.addPoint(396, 483);

	// Both triangles illegal. (i.e. contains point)
	EXPECT_EQ(1, pointSet.inCircle(p1, p2, p4, p3));
	EXPECT_EQ(1, pointSet.inCircle(p4, p2, p3, p1));
}
