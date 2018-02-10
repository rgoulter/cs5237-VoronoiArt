#include <vector>

#include "catch.hpp"

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/directedgraph.h"

using std::vector;

using namespace delaunay;

#define SUT_NAME "delaunay/DirectedGraph"
#define SUT_TAGS "[delaunay][DirectedGraph]"



// NOTE: 2018-02-05: At the moment, this test case doesn't do anything.
TEST_CASE(SUT_NAME ", Trivial", SUT_TAGS) {
	PointSetArray<LongInt> pointSet;

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
