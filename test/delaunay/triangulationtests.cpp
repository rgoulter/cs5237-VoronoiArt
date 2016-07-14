#include <vector>

#include "gtest/gtest.h"

#include "delaunay/directedgraph.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/triangulation.h"

using std::vector;
using std::cout;
using std::endl;

using namespace delaunay;



TEST(TriangulationTest, TrivialLinkTrisCase) {
	// Arbitrary, so long as distinct.
	int iIdx = 1;
	int jIdx = 2;
	int kIdx = 3;
	int lIdx = 4;

	TriRecord triIJK(iIdx, jIdx, kIdx);
	TriRecord triILJ(iIdx, lIdx, jIdx);

	Triangulation trist;

	FIndex ijkIdx = trist.addLinkedTri(triIJK);
	FIndex iljIdx = trist.addLinkedTri(triILJ);

	trist.setLink(ijkIdx, 0, iljIdx); // link by edge IJ

	const LinkedTriangle& ltriIJK = trist[ijkIdx];
	const LinkedTriangle& ltriILJ = trist[iljIdx];

	EXPECT_EQ(iljIdx, ltriIJK.links_[0]);
	EXPECT_EQ(ijkIdx, ltriILJ.links_[2]);
}



// Does it work with another rotation of the ILJ tri?
TEST(TriangulationTest, TrivialLinkTrisCase2) {
	// Arbitrary, so long as distinct.
	int iIdx = 1;
	int jIdx = 2;
	int kIdx = 3;
	int lIdx = 4;

	TriRecord triIJK(iIdx, jIdx, kIdx);
	TriRecord triLJI(lIdx, jIdx, iIdx);

	Triangulation trist;

	FIndex ijkIdx = trist.addLinkedTri(triIJK);
	FIndex ljiIdx = trist.addLinkedTri(triLJI);

	trist.setLink(ijkIdx, 0, ljiIdx); // link by edge IJ

	const LinkedTriangle& ltriIJK = trist[ijkIdx];
	const LinkedTriangle& ltriLJI = trist[ljiIdx];

	EXPECT_EQ(ljiIdx, ltriIJK.links_[0]);
	EXPECT_EQ(ijkIdx, ltriLJI.links_[1]);
}

