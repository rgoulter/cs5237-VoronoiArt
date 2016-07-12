#include <vector>

#include "gtest/gtest.h"

#include "delaunay/li.h"
#include "delaunay/polygon.h"

using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(PointSetTest, MyPointPolyPointInPolygon) {
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

	EXPECT_EQ(1, inPoly(poly, p5));
	EXPECT_EQ(0, inPoly(poly, p6));
}



TEST(PolygonTest, PolyPointInIntPolygon) {
	int p1x =  0, p1y =  0;
	int p2x =  0, p2y = 10;
	int p3x = 10, p3y = 10;
	int p4x = 10, p4y =  0;
	int p5x =  5, p5y =  5;
	int p6x = 15, p6y = 15;

	std::vector<int> poly;
	poly.push_back(p1x); poly.push_back(p1y);
	poly.push_back(p2x); poly.push_back(p2y);
	poly.push_back(p3x); poly.push_back(p3y);
	poly.push_back(p4x); poly.push_back(p4y);

	EXPECT_EQ(1, inPoly(poly, p5x, p5y));
	EXPECT_EQ(0, inPoly(poly, p6x, p6y));
}




TEST(PolygonTest, LineSegIsectIntPointBasic) {
	int ax =   0, ay =  0;
	int bx =  10, by = 10;
	int cx =   0, cy =  2;
	int dx =   2, dy =  0;
	int ix, iy;

	// Should be this
	int eix =  1, eiy =  1;

	findIntersectionPoint(ax, ay, bx, by, cx, cy, dx, dy, ix, iy);

	EXPECT_EQ(eix, ix);
	EXPECT_EQ(eiy, iy);
}



TEST(PolygonTest, ClipPolyRectCaseNoIsect) {
	int x1 =   0, x2 = 100;
	int y1 =   0, y2 = 100;

	// Test polygon (clearly within the outer rect).
	vector<int> testPoly;
	testPoly.push_back(10); testPoly.push_back(10);
	testPoly.push_back(20); testPoly.push_back(10);
	testPoly.push_back(20); testPoly.push_back(20);
	testPoly.push_back(10); testPoly.push_back(20);

	vector<int> output = clipPolygonToRectangle(testPoly, x1, y1, x2, y2);

	EXPECT_EQ(testPoly.size(), output.size());

	for (unsigned int i = 0; i < testPoly.size(); i += 2) {
		EXPECT_EQ(testPoly[i], output[i]);
		EXPECT_EQ(testPoly[i + 1], output[i + 1]);
	}
}



TEST(PolygonTest, ClipPolyRectCaseSimpleIsect) {
	int x1 =   0, x2 = 100;
	int y1 =   0, y2 = 100;

	// Test polygon (clearly outside of rect).
	vector<int> testPoly;
	testPoly.push_back(50); testPoly.push_back(50);
	testPoly.push_back(50); testPoly.push_back(-250);
	testPoly.push_back(250); testPoly.push_back(50);

	// Expected polygon
	vector<int> expectedPoly;
	expectedPoly.push_back(50);  expectedPoly.push_back(0);
	expectedPoly.push_back(100); expectedPoly.push_back(0);
	expectedPoly.push_back(100); expectedPoly.push_back(50);
	expectedPoly.push_back(50);  expectedPoly.push_back(50);

	vector<int> output = clipPolygonToRectangle(testPoly, x1, y1, x2, y2);

	if (expectedPoly.size() != output.size()) {
		cout << "Expected poly has " << (expectedPoly.size() / 2) << "points," << endl;
		cout << "Output poly different than expected:" << endl;
	}

	// cout << "Output poly has " << (output.size() / 2) << "points," << endl;
	// for (unsigned int i = 0; i < output.size() / 2; i++) {
	// 	cout << output[2 * i] << ","  << output[2 * i + 1]<< endl;
	// }

	// MUST be eq.
	ASSERT_EQ(expectedPoly.size(), output.size());

	for (unsigned int i = 0; i < expectedPoly.size(); i += 2) {
		EXPECT_EQ(expectedPoly[i], output[i]);
		EXPECT_EQ(expectedPoly[i + 1], output[i + 1]);
	}
}

