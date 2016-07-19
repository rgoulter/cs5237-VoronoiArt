#include <vector>

#include "gtest/gtest.h"

#include "delaunay/li.h"

#include "geometry/linesegment.h"
#include "geometry/polygon.h"

using std::pair;
using std::vector;
using std::cout;
using std::endl;

using namespace geometry;



TEST(PointSetTest, IntPointPolyPointInPolygon) {
	Point<int> p1( 0,  0);   // 1
	Point<int> p2( 0, 10);   // 2
	Point<int> p3(10, 10);   // 3
	Point<int> p4(10,  0);   // 4
	Point<int> p5( 5,  5);   // 5
	Point<int> p6(15, 15);   // 6

	Polygon poly;
	poly.addPoint(p1);
	poly.addPoint(p2);
	poly.addPoint(p3);
	poly.addPoint(p4);

	EXPECT_EQ(1, inPoly(poly, p5));
	EXPECT_EQ(0, inPoly(poly, p6));
}



TEST(PolygonTest, LineSegIsectIntPointBasic) {
	Point<int> a = {  0,  0 };
	Point<int> b = { 10, 10 };
	Point<int> c = {  0,  2 };
	Point<int> d = {  2,  0 };

	// Should be this
	Point<int> expectedIP(1,1);

	Point<int> actualIP =
		findIntersectionPoint({ a, b }, { c, d });

	EXPECT_EQ(expectedIP, actualIP);
}



TEST(PolygonTest, ClipPolyRectCaseNoIsect) {
	Rect clipRect({0, 0}, 100, 100);

	// Test polygon (clearly within the outer rect).
	Polygon testPoly;
	testPoly.addPoint(10, 10);
	testPoly.addPoint(20, 10);
	testPoly.addPoint(20, 20);
	testPoly.addPoint(10, 20);

	Polygon output = clipPolygonToRectangle(testPoly, clipRect);

	EXPECT_EQ(testPoly.numPoints(), output.numPoints());

	for (unsigned int i = 0; i < testPoly.numPoints(); ++i) {
		EXPECT_EQ(testPoly[i], output[i]);
	}
}



TEST(PolygonTest, ClipPolyRectCaseNoIsectOutside) {
	Rect clipRect({0, 0}, 100, 100);

	// Test polygon (clearly OUTSIDE the outer rect).
	Polygon testPoly;
	testPoly.addPoint(1010, 10);
	testPoly.addPoint(1020, 10);
	testPoly.addPoint(1020, 20);
	testPoly.addPoint(1010, 20);

	Polygon output = clipPolygonToRectangle(testPoly, clipRect);

	EXPECT_EQ((unsigned int)0, output.numPoints());
}



TEST(PolygonTest, ClipPolyRectCaseSimpleIsect) {
	Rect clipRect({0, 0}, 100, 100);

	// Test polygon (clearly intersects with rect).
	Polygon testPoly;
	testPoly.addPoint( 50,   50);
	testPoly.addPoint( 50, -250);
	testPoly.addPoint(250,   50);

	// Expected polygon
	Polygon expectedPoly;
	expectedPoly.addPoint( 50,  0); // 1st i'sect pt.
	expectedPoly.addPoint(100,  0);
	expectedPoly.addPoint(100, 50);
	expectedPoly.addPoint( 50, 50);

	Polygon output = clipPolygonToRectangle(testPoly, clipRect);

	if (expectedPoly.numPoints() != output.numPoints()) {
		cout << "Expected poly has " << expectedPoly.numPoints() << " points," << endl;
		cout << "Output poly different than expected:" << endl;
	}

	// cout << "Expected poly:" << endl;
	// for (const Point<int>& pt : expectedPoly.points()) {
	// 	cout << pt << endl;
	// }
    //
	// cout << "Output poly:" << endl;
	// for (const Point<int>& pt : output.points()) {
	// 	cout << pt << endl;
	// }

	// cout << "Output poly has " << (output.size() / 2) << "points," << endl;
	// for (unsigned int i = 0; i < output.size() / 2; i++) {
	// 	cout << output[2 * i] << ","  << output[2 * i + 1]<< endl;
	// }

	// MUST be eq.
	ASSERT_EQ(expectedPoly.numPoints(), output.numPoints());

	for (unsigned int i = 0; i < expectedPoly.numPoints(); ++i) {
		EXPECT_EQ(expectedPoly[i], output[i]);
	}
}



TEST(PolygonTest, ClipPolyRectCaseIsectEdgeTouching) {
	Rect clipRect({0, 0}, 100, 100);

	// Test polygon: Touches the edge, but never outside
	Polygon testPoly;
	testPoly.addPoint( 50,  50);
	testPoly.addPoint( 50, 100);
	testPoly.addPoint(  0, 100);

	// Expected polygon (exactly the same)
	Polygon expectedPoly;
	expectedPoly.addPoint( 50,  50);
	expectedPoly.addPoint( 50, 100);
	expectedPoly.addPoint(  0, 100);

	Polygon output = clipPolygonToRectangle(testPoly, clipRect);

	if (expectedPoly.numPoints() != output.numPoints()) {
		cout << "Expected poly has " << expectedPoly.numPoints() << " points," << endl;
		cout << "Output poly different than expected:" << endl;
	}

	// cout << "Expected poly:" << endl;
	// for (const Point<int>& pt : expectedPoly.points()) {
	// 	cout << pt << endl;
	// }
    //
	// cout << "Output poly:" << endl;
	// for (const Point<int>& pt : output.points()) {
	// 	cout << pt << endl;
	// }

	// cout << "Output poly has " << (output.size() / 2) << "points," << endl;
	// for (unsigned int i = 0; i < output.size() / 2; i++) {
	// 	cout << output[2 * i] << ","  << output[2 * i + 1]<< endl;
	// }

	// MUST be eq.
	ASSERT_EQ(expectedPoly.numPoints(), output.numPoints());

	for (unsigned int i = 0; i < expectedPoly.numPoints(); ++i) {
		EXPECT_EQ(expectedPoly[i], output[i]);
	}
}



TEST(PolygonTest, ClipPolyRectCaseIsectEdgeTouchingOutside) {
	Rect clipRect({0, 0}, 100, 100);

	// Test polygon: touches edge, has some points outside.
	Polygon testPoly;
	testPoly.addPoint( 50,   50);
	testPoly.addPoint( 50,  100);
	testPoly.addPoint(-50,  100);
	testPoly.addPoint(  0,   50);

	// Expected polygon
	Polygon expectedPoly;
	expectedPoly.addPoint(  0, 100);
	expectedPoly.addPoint(  0,  50);
	expectedPoly.addPoint( 50,  50);
	expectedPoly.addPoint( 50, 100);

	Polygon output = clipPolygonToRectangle(testPoly, clipRect);

	if (expectedPoly.numPoints() != output.numPoints()) {
		cout << "Expected poly has " << expectedPoly.numPoints() << " points," << endl;
		cout << "Output poly different than expected:" << endl;
	}

	// cout << "Expected poly:" << endl;
	// for (const Point<int>& pt : expectedPoly.points()) {
	// 	cout << pt << endl;
	// }
    //
	// cout << "Output poly:" << endl;
	// for (const Point<int>& pt : output.points()) {
	// 	cout << pt << endl;
	// }

	// cout << "Output poly has " << (output.size() / 2) << "points," << endl;
	// for (unsigned int i = 0; i < output.size() / 2; i++) {
	// 	cout << output[2 * i] << ","  << output[2 * i + 1]<< endl;
	// }

	// MUST be eq.
	ASSERT_EQ(expectedPoly.numPoints(), output.numPoints());

	for (unsigned int i = 0; i < expectedPoly.numPoints(); ++i) {
		EXPECT_EQ(expectedPoly[i], output[i]);
	}
}
