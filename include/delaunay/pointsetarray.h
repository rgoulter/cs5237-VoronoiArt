#ifndef DELAUNAY_POINTSETARRAYH
#define DELAUNAY_POINTSETARRAYH

#include <vector>

#include "delaunay/longint/li.h"
#include "delaunay/longint/lmath.h"

#include "delaunay/polygon.h"



namespace delaunay {

class PointSetArray {
public:
	/// add an ith (according to the adding sequence) point (x1, y1),
	/// return i.
	int addPoint(LongInt x1, LongInt y1);

	/// put the x,y values into x1,y1, and return 1 if the point pIndex exists
	int getPoint (int pIndex, LongInt& x1, LongInt& y1) const;

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
	bool circumCircle(int p1Idx, int p2Idx, int p3Idx, MyPoint& center) const;

	/// erase all points in this array
	void eraseAllPoints();

	const MyPoint& operator[](int idx) const;

private:
	std::vector<MyPoint> myPoints;
};



std::vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa);

}

#endif
