#include <vector>

#include "catch.hpp"

#include "delaunay/directedgraph.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/triangulation.h"

using std::vector;

using namespace delaunay;

#define SUT_NAME "delaunay/Triangulation"
#define SUT_TAGS "[delaunay][LinkedTriangle][Triangulation][addLinkedTri]"



TEST_CASE(SUT_NAME "/addLinkedTri, Check Links", SUT_TAGS) {
	// Arbitrary, so long as distinct.
	int iIdx = 1;
	int jIdx = 2;
	int kIdx = 3;
	int lIdx = 4;

	TriRecord triIJK(iIdx, jIdx, kIdx);
	TriRecord triILJ(iIdx, lIdx, jIdx);
	TriRecord triLJI(lIdx, jIdx, iIdx);

	Triangulation trist;

	SECTION("Triangles IJK, ILJ linked by edge IJ") {
		FIndex ijkIdx = trist.addLinkedTri(triIJK);
		FIndex iljIdx = trist.addLinkedTri(triILJ);

		trist.setLink(ijkIdx, 0, iljIdx); // link by edge IJ

		const LinkedTriangle& ltriIJK = trist[ijkIdx];
		const LinkedTriangle& ltriILJ = trist[iljIdx];

		CHECK(iljIdx == ltriIJK.links_[0]);
		CHECK(ijkIdx == ltriILJ.links_[2]);
	}

	SECTION("Triangles IJK, LJI linked by edge IJ") {
		FIndex ijkIdx = trist.addLinkedTri(triIJK);
		FIndex ljiIdx = trist.addLinkedTri(triLJI);

		trist.setLink(ijkIdx, 0, ljiIdx); // link by edge IJ

		const LinkedTriangle& ltriIJK = trist[ijkIdx];
		const LinkedTriangle& ltriLJI = trist[ljiIdx];

		CHECK(ljiIdx == ltriIJK.links_[0]);
		CHECK(ijkIdx == ltriLJI.links_[1]);
	}
}
