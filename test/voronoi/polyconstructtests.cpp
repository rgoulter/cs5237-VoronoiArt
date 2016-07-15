#include <cmath>

#include <vector>

#include "gtest/gtest.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/directedgraph.h"

#include "voronoi/polyconstruct.h"

using std::vector;
using std::cout;
using std::endl;

using delaunay::PointSetArray;

using namespace voronoi;



TEST(VoronoiPolyConstructTest, LinkEdgesTrivialCase) {
	// VEdge isn't quite pair<Pt,Pt>; needs 'start'
	VPoint *s = new VPoint(0, 0);

	VPoint *a = new VPoint(10, 10);
	VPoint *b = new VPoint(50, 50);
	VPoint *c = new VPoint(90, 10);

	VEdge *ab = new VEdge(s, a, b);
	VEdge *bc = new VEdge(s, b, c);

	Edges edges;
	edges.push_back(ab);
	edges.push_back(bc);

	vector<HalfWingedEdge*> linkedEdges = linkEdges(edges);

	ASSERT_EQ((size_t)4, linkedEdges.size());

	// Figure out which edge is where
	// (Could prob'ly use a map<> here).
	HalfWingedEdge *linkedEdgeAB;
	HalfWingedEdge *linkedEdgeBA;
	HalfWingedEdge *linkedEdgeBC;
	HalfWingedEdge *linkedEdgeCB;

	for (HalfWingedEdge *linkedEdge : linkedEdges) {
		if (linkedEdge->pointIdxA_ == a &&
		    linkedEdge->pointIdxB_ == b) {
			linkedEdgeAB = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == b &&
		    linkedEdge->pointIdxB_ == a) {
			linkedEdgeBA = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == b &&
		    linkedEdge->pointIdxB_ == c) {
			linkedEdgeBC = linkedEdge;
		}
		if (linkedEdge->pointIdxA_ == c &&
		    linkedEdge->pointIdxB_ == b) {
			linkedEdgeCB = linkedEdge;
		}
	}

	ASSERT_TRUE(linkedEdgeAB != nullptr);
	ASSERT_TRUE(linkedEdgeBA != nullptr);
	ASSERT_TRUE(linkedEdgeBC != nullptr);
	ASSERT_TRUE(linkedEdgeCB != nullptr);

	//
	// assert the angles are as I'd expect them.
	// n.b. atan2 range is from -PI to +PI
	//
	const double angle45 = atan(1);
	ASSERT_EQ( 1, linkedEdgeAB->abAngle_ / angle45);
	ASSERT_EQ(-3, linkedEdgeBA->abAngle_ / angle45);

	ASSERT_EQ(-1, linkedEdgeBC->abAngle_ / angle45);
	ASSERT_EQ( 3, linkedEdgeCB->abAngle_ / angle45);

	//
	// Now assert that the function linked the right things together
	//

	EXPECT_EQ(nullptr,      linkedEdgeAB->prevEdge_);

	EXPECT_EQ(linkedEdgeBC, linkedEdgeAB->nextEdge_);
	EXPECT_EQ(linkedEdgeAB, linkedEdgeBC->prevEdge_);

	EXPECT_EQ(nullptr,      linkedEdgeBC->nextEdge_);

	EXPECT_EQ(nullptr,      linkedEdgeCB->prevEdge_);

	EXPECT_EQ(linkedEdgeBA, linkedEdgeCB->nextEdge_);
	EXPECT_EQ(linkedEdgeCB, linkedEdgeBA->prevEdge_);

	EXPECT_EQ(nullptr,      linkedEdgeBA->nextEdge_);

	// Clean up...
	delete linkedEdgeAB;
	delete linkedEdgeBA;
	delete linkedEdgeBC;
	delete linkedEdgeCB;
	delete ab;
	delete bc;
	delete a;
	delete b;
	delete c;
}



// Could test polygonFromLinkedEdge as intermediate


