#include "triangle.h"

#include <assert.h>

#include <iostream>

#include "li.h"
#include "pointsetarray.h"

using std::cout;
using std::endl;



TriRecord::TriRecord(int idx1, int idx2, int idx3) {
	assert(idx1 >= 1 && idx1 < 1000000);
	assert(idx2 >= 1 && idx2 < 1000000);
	assert(idx3 >= 1 && idx3 < 1000000);
	vi_[0] = idx1;
	vi_[1] = idx2;
	vi_[2] = idx3;
}



void TriRecord::nextEdge(int& commonIdx, int& nextIdx, const TriRecord& nextTri) const {
	int commonVert = vi_[commonIdx];
	int notVert    = vi_[nextIdx]; // don't want nextIdx

	if (nextTri.vi_[0] == commonVert &&
		nextTri.vi_[1] != notVert) {
		commonIdx = 0;
		nextIdx   = 1;
	} else if (nextTri.vi_[0] != notVert &&
	           nextTri.vi_[1] == commonVert) {
		commonIdx = 1;
		nextIdx   = 0;
	} else if (nextTri.vi_[1] == commonVert &&
	           nextTri.vi_[2] != notVert) {
		commonIdx = 1;
		nextIdx   = 2;
	} else if (nextTri.vi_[1] != notVert &&
	           nextTri.vi_[2] == commonVert) {
		commonIdx = 2;
		nextIdx   = 1;
	} else if (nextTri.vi_[2] == commonVert &&
	           nextTri.vi_[0] != notVert) {
		commonIdx = 2;
		nextIdx   = 0;
	} else if (nextTri.vi_[2] != notVert &&
	           nextTri.vi_[0] == commonVert) {
		commonIdx = 0;
		nextIdx   = 2;
	}
}



std::ostream& operator<<(std::ostream& os, const TriRecord& tri) {
	os << "Tri(" << tri.vi_[0] << "," << tri.vi_[1] << "," << tri.vi_[2] << ")";
	return os;
}



// Adds the points to the PointSetArray
void findBoundingTri(PointSetArray &pSet) {
	assert(pSet.noPt() > 0);

	LongInt minX, minY;
	pSet.getPoint(1, minX, minY);
	LongInt maxX, maxY;
	pSet.getPoint(1, maxX, maxY);

	// MAGIC value of 2000; may not be enough?
	LongInt thousand = 2000;

	for (int i = 1; i < pSet.noPt(); i++) {
		LongInt x, y;
		pSet.getPoint(i, x, y);

		if (minX > x)
			minX = x;
		else if (maxX < x)
			maxX = x;

		if (minY > y)
		    minY = y;
		else if (maxY < y)
		    maxY = y;
	}

	LongInt delta = 5;

	minX = minX - delta - thousand;
	maxX = maxX + delta + thousand;
	minY = minY - delta - thousand;
	maxY = maxY + delta + thousand;

	// TODO This seems nonsense. Does it work?

	int super1Idx = pSet.addPoint(minX - (maxY - minY), minY);
	int super2Idx = pSet.addPoint(maxX + (maxY - minY), minY);

	maxX = (maxX.doubleValue() - minX.doubleValue()) / 2;

	// some rounding may occur if LongInt is odd
	int super3Idx = pSet.addPoint((LongInt) ((maxX.doubleValue() + minX.doubleValue()) / 2),
	                              maxY + (maxX - minX));

	// Assert that all (other) points contained in the 3 super-triangle.
	assert(isTriangleCCW(pSet, TriRecord(super1Idx, super2Idx, super3Idx)));
	for (int i = 1; i < pSet.noPt() - 3; i++) {
		assert(pSet.inTri(super1Idx, super2Idx, super3Idx, i) > 0);
	}
}



bool isTriangleCCW(const PointSetArray& psa, const TriRecord& tri) {
	int pIdx1, pIdx2, pIdx3;
	tri.get(pIdx1, pIdx2, pIdx3);

	// TODO: want easy way to get MyPoint from PointSetArray
	LongInt p1x, p1y;
	psa.getPoint(pIdx1, p1x, p1y);

	LongInt p2x, p2y;
	psa.getPoint(pIdx2, p2x, p2y);

	LongInt p3x, p3y;
	psa.getPoint(pIdx3, p3x, p3y);

	MyPoint p1(p1x, p1y);
	MyPoint p2(p2x, p2y);
	MyPoint p3(p3x, p3y);

	// Test that p3 is ccw to p1p2,
	//           p1 is ccw to p2p3,
	//           p2 is ccw to p3p1
	bool isCCWp2p3 = orientation(p1, p2, p3) == 1;
	bool isCCWp3p1 = orientation(p2, p3, p1) == 1;
	bool isCCWp1p2 = orientation(p3, p1, p2) == 1;

	return isCCWp2p3 && isCCWp3p1 && isCCWp1p2;
}



inline bool isects(const MyPoint& a, const MyPoint& b, const MyPoint& c, const MyPoint& d) {
	// Incidental (touching) returns false.
	return (orientation(a, b, c) * orientation(a, b, d) < 0) &&
	       (orientation(c, d, a) * orientation(c, d, b) < 0);
}



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri, int pIdx1, int pIdx2) {
	int idx1, idx2, idx3;
	tri.get(idx1, idx2, idx3);

	MyPoint p1 = psa[idx1];
	MyPoint p2 = psa[idx2];
	MyPoint p3 = psa[idx3];

	MyPoint e1 = psa[pIdx1];
	MyPoint e2 = psa[pIdx2];

	bool isects12 = isects(p1, p2, e1, e2);
	bool isects23 = isects(p2, p3, e1, e2);
	bool isects31 = isects(p3, p1, e1, e2);

	return isects12 || isects23 || isects31;
}



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri1, const TriRecord& tri2) {
	int idx1, idx2, idx3;
	tri2.get(idx1, idx2, idx3);

	bool isects12 = intersectsTriangle(psa, tri1, idx1, idx2);
	bool isects23 = intersectsTriangle(psa, tri1, idx2, idx3);
	bool isects31 = intersectsTriangle(psa, tri1, idx3, idx1);

	return isects12 || isects23 || isects31;
}

