#include "gtest/gtest.h"
#include "li.h"
#include "pointSet.h"
#include <vector>

TEST(BasicsTest, PointSetInTriSimple) {
	PointSet ps;
	
	int p1 = ps.addPoint(0, 0);   // 1
	int p2 = ps.addPoint(0, 4);   // 2
	int p3 = ps.addPoint(4, 0);   // 3
	int p4 = ps.addPoint(4, 4);   // 4
	int p5 = ps.addPoint(1, 1);   // 5

	// In case the above goes out of range
	ps.addPoint(1, 1);   // 6

	// (1, 1) IS in tri 123
	EXPECT_EQ(1, ps.inTri(p1, p2, p3, p5));
	
	// (1, 1) IS in tri 124
	EXPECT_EQ(0, ps.inTri(p1, p2, p4, p5));
	
	// (1, 1) IS NOT in tri 234
	EXPECT_EQ(-1, ps.inTri(p2, p3, p4, p5));
}

TEST(BasicsTest, PolyPointInPolygon) {
	PointSet ps;
	
	MyPoint p1( 0,  0);   // 1
	MyPoint p2( 0, 10);   // 2
	MyPoint p3(10, 10);   // 3
	MyPoint p4(10,  0);   // 4
	MyPoint p5( 5,  5);   // 5
	MyPoint p6(15, 15);   // 6

	std::vector<MyPoint> poly;
	poly.push_back(p1);
	poly.push_back(p2);
	poly.push_back(p3);
	poly.push_back(p4);

	// (1, 1) IS in tri 123
	EXPECT_EQ(1, inPoly(poly, p5));
	
	// (1, 1) IS in tri 124
	EXPECT_EQ(0, inPoly(poly, p6));
}



int main(int argc, char* argv[]) {
	// Could skip defining this by linking to GTest's src/gtest_main.cc
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}