#include <vector>

#include "gtest/gtest.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/directedgraph.h"

using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(DelaunayDAGTest, TrivialCase) {
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

