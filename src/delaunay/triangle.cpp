#include "delaunay/triangle.h"
#include "delaunay/trianglegeometry.h"

#include <assert.h>

#include <iostream>
#include <utility>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"

#include "geometry/linesegment.h"

#ifndef NDEBUG
#define TRIANGLE_CHECK
#endif

using std::pair;
using std::cout;
using std::endl;

using geometry::intersects;
using geometry::orientation;
using geometry::Intersection;
using geometry::Point;



namespace delaunay {

TriRecord::TriRecord(int idx1, int idx2, int idx3) {
#ifdef TRIANGLE_CHECK
	assert(idx1 >= 1 && idx1 < 1000000);
	assert(idx2 >= 1 && idx2 < 1000000);
	assert(idx3 >= 1 && idx3 < 1000000);
#endif
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



// Assuming tri1, tri2 share an edge,
// get the point indices from the two triangles.
void getIndicesKIJL(const TriRecord& triIJK, const TriRecord& triILJ,
                    int& kIdx, int& iIdx, int& jIdx, int& lIdx) {
	lIdx = triIJK.vertexNotSharedWith(triILJ);

	int pIdx1, pIdx2, pIdx3;
	triIJK.get(pIdx1, pIdx2, pIdx3);

	if (!triILJ.hasPointIndex(pIdx1)) {
		iIdx = pIdx2;
		jIdx = pIdx3;
		kIdx = pIdx1;
	} else if (!triILJ.hasPointIndex(pIdx2)) {
		iIdx = pIdx3;
		jIdx = pIdx1;
		kIdx = pIdx2;
	} else if (!triILJ.hasPointIndex(pIdx3)) {
		iIdx = pIdx1;
		jIdx = pIdx2;
		kIdx = pIdx3;
	} else {
	}
}



// Adds the points to the PointSetArray
template<typename I>
void findBoundingTri(PointSetArray<I> &pSet) {
	assert(pSet.noPt() > 0);

	I minX, minY;
	pSet.getPoint(1, minX, minY);
	I maxX, maxY;
	pSet.getPoint(1, maxX, maxY);

	// MAGIC value of 2000; may not be enough?
	I thousand = 2000;

	for (int i = 1; i < pSet.noPt(); i++) {
		I x, y;
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

	I delta = 5;

	minX = minX - delta - thousand;
	maxX = maxX + delta + thousand;
	minY = minY - delta - thousand;
	maxY = maxY + delta + thousand;

	// TODO This seems nonsense. Does it work?

	int super1Idx = pSet.addPoint(minX - (maxY - minY), minY);
	int super2Idx = pSet.addPoint(maxX + (maxY - minY), minY);

	maxX = ((double)maxX - (double)minX) / 2;

	// some rounding may occur if LongInt is odd
	int super3Idx = pSet.addPoint((I) (((double) maxX + (double) minX) / 2),
	                              maxY + (maxX - minX));

#ifdef TRIANGLE_CHECK
	// Assert that all (other) points contained in the 3 super-triangle.
	assert(isTriangleCCW(pSet, TriRecord(super1Idx, super2Idx, super3Idx)));
	for (int i = 1; i < pSet.noPt() - 3; i++) {
		assert(pSet.inTri(super1Idx, super2Idx, super3Idx, i) > 0);
	}
#endif
}



template<typename I>
int inTriangle(const PointSetArray<I>& psa, const TriRecord& tri, int pIdx) {
	int pIdx1, pIdx2, pIdx3;
	tri.get(pIdx1, pIdx2, pIdx3);

	return psa.inTri(pIdx1, pIdx2, pIdx3, pIdx);
}



// again, positive-y points UP.
template<typename I>
bool isTriangleCCW(const PointSetArray<I>& psa, const TriRecord& tri) {
	int pIdx1, pIdx2, pIdx3;
	tri.get(pIdx1, pIdx2, pIdx3);

	// TODO: want easy way to get MyPoint from PointSetArray
	I p1x, p1y;
	psa.getPoint(pIdx1, p1x, p1y);

	I p2x, p2y;
	psa.getPoint(pIdx2, p2x, p2y);

	I p3x, p3y;
	psa.getPoint(pIdx3, p3x, p3y);

	Point<I> p1(p1x, p1y);
	Point<I> p2(p2x, p2y);
	Point<I> p3(p3x, p3y);

	// Test that p3 is ccw to p1p2,
	//           p1 is ccw to p2p3,
	//           p2 is ccw to p3p1
	bool isCCWp2p3 = orientation({p1, p2}, p3) == 1;
	bool isCCWp3p1 = orientation({p2, p3}, p1) == 1;
	bool isCCWp1p2 = orientation({p3, p1}, p2) == 1;

	return isCCWp2p3 && isCCWp3p1 && isCCWp1p2;
}



template<typename I>
Intersection intersectsTriangle(const PointSetArray<I>& psa, const TriRecord& tri, pair<int,int> idxPt) {
	int pIdx1 = idxPt.first;
	int pIdx2 = idxPt.second;

	int idx1, idx2, idx3;
	tri.get(idx1, idx2, idx3);

	const Point<I>& p1 = psa[idx1];
	const Point<I>& p2 = psa[idx2];
	const Point<I>& p3 = psa[idx3];

	const Point<I>& e1 = psa[pIdx1];
	const Point<I>& e2 = psa[pIdx2];

	using geometry::isOverlapping;

	// compiler has trouble inferring the type here, so be explicit
	using LineSeg = geometry::LineSegment<I>;
	const LineSeg& e1e2 = {e1, e2};
	const LineSeg& p1p2 = {p1, p2};
	const LineSeg& p2p3 = {p2, p3};
	const LineSeg& p3p1 = {p3, p1};

	Intersection isects12 = intersects(p1p2, e1e2);
	Intersection isects23 = intersects(p2p3, e1e2);
	Intersection isects31 = intersects(p3p1, e1e2);

	if (isOverlapping(isects12) ||
	    isOverlapping(isects23) ||
	    isOverlapping(isects31)) {
	    return Intersection::Overlap;
	} else if (isTouching(isects12) ||
	           isTouching(isects23) ||
	           isTouching(isects31)) {
		return Intersection::Incidental;
	} else {
		return Intersection::None;
	}
}



template<typename I>
Intersection intersectsTriangle(const PointSetArray<I>& psa, const TriRecord& tri1, const TriRecord& tri2) {
	int idx1, idx2, idx3;
	tri2.get(idx1, idx2, idx3);

	Intersection isects12 = intersectsTriangle(psa, tri1, {idx1, idx2});
	Intersection isects23 = intersectsTriangle(psa, tri1, {idx2, idx3});
	Intersection isects31 = intersectsTriangle(psa, tri1, {idx3, idx1});

	if (isOverlapping(isects12) ||
	    isOverlapping(isects23) ||
	    isOverlapping(isects31)) {
	    return Intersection::Overlap;
	} else if (isTouching(isects12) ||
	           isTouching(isects23) ||
	           isTouching(isects31)) {
		return Intersection::Incidental;
	} else {
		return Intersection::None;
	}
}

// Instantiate functions
template void findBoundingTri<LongInt>(PointSetArray<LongInt>&);
template int inTriangle<LongInt>(const PointSetArray<LongInt>&, const TriRecord&, int);
template bool isTriangleCCW<LongInt>(const PointSetArray<LongInt>&, const TriRecord&);
template Intersection intersectsTriangle<LongInt>(const PointSetArray<LongInt>&, const TriRecord&, pair<int,int>);
template Intersection intersectsTriangle<LongInt>(const PointSetArray<LongInt>&, const TriRecord&, const TriRecord&);

template void findBoundingTri<double>(PointSetArray<double>&);
template int inTriangle<double>(const PointSetArray<double>&, const TriRecord&, int);
template bool isTriangleCCW<double>(const PointSetArray<double>&, const TriRecord&);
template Intersection intersectsTriangle<double>(const PointSetArray<double>&, const TriRecord&, pair<int,int>);
template Intersection intersectsTriangle<double>(const PointSetArray<double>&, const TriRecord&, const TriRecord&);

}


