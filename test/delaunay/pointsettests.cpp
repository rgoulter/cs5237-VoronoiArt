#include <vector>

#include "gtest/gtest.h"

#include "pointsetarray.h"
#include "directedgraph.h"

using std::vector;
using std::cout;
using std::endl;



TEST(PointSetTest, PointSetInTriSimple) {
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


TEST(PointSetTest, InCircle) {
	PointSetArray pointSet;

	pointSet.addPoint(0, 100);
	pointSet.addPoint(100, 0);
	pointSet.addPoint(200, 100);
	pointSet.addPoint(0, 100);


	// EXPECT_EQ(testPoly.size(), output.size());
	//
	// for (unsigned int i = 0; i < testPoly.size(); i += 2) {
	// 	EXPECT_EQ(testPoly[i], output[i]);
	// 	EXPECT_EQ(testPoly[i + 1], output[i + 1]);
	// }
}


