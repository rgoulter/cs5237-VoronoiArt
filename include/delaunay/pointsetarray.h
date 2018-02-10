#ifndef DELAUNAY_POINTSETARRAYH
#define DELAUNAY_POINTSETARRAYH

#include <vector>

#include "geometry/point.h"



namespace delaunay {

template<typename I>
class PointSetArray {
public:
	/// add an ith (according to the adding sequence) point (x1, y1),
	/// return i.
	int addPoint(I x1, I y1);

	// XXX: 2018-02-10, Richard: This is a stupid signature. -- Should use [] instead. (Which exists).
	/// put the x,y values into x1,y1, and return 1 if the point pIndex exists
	int getPoint (int pIndex, I& x1, I& y1) const;

	/// return the number of points
	int noPt() const;

	/// return 1 if the point p is inside the triangle p1p2p3,
	///  by reference of the indices, -1 if outside, 0 if degenerate
	int inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx) const;

	/// return 1 if the point p is inside the circumcircle of p1p2p3,
	/// -1 if outside,
	/// 0 if degenerate
	int inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) const;

	/// returns true if the 3 input points are not co-linear.
	bool circumCircle(int p1Idx, int p2Idx, int p3Idx, geometry::Point<I>& center) const;

	/// erase all points in this array
	void eraseAllPoints();

	const geometry::Point<I>& operator[](int idx) const;

private:
	std::vector<geometry::Point<I>> myPoints;
};



template<typename I>
std::vector<int> coercePSAPolyToIVecPoly(PointSetArray<I>& psa);

}

#endif
