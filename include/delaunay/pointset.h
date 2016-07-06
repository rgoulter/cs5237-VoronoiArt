#ifndef POINTSETH
#define POINTSETH

#include <vector>

#include "li.h"
#include "lmath.h"
#include "polygon.h"



class PointSet {
protected:
	std::vector<MyPoint> myPoints;

public:

	/// add an ith (according to the adding sequence) point (x1, y1),
	/// return i.
	int addPoint(LongInt x1, LongInt y1);

	void deleteLastPoint();

	/// return 1 if the point p is inside the triangle p1p2p3,
	///  by reference of the indices, -1 if outside, 0 if degenerate
	int inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx);

	/// return 1 if the point p is inside the circumcircle of p1p2p3,
	/// -1 if outside,
	/// 0 if degenerate
	int inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx);

	/// returns true if the 3 input points are not co-linear.
	bool circumCircle(int p1Idx, int p2Idx, int p3Idx, MyPoint& center);

};
#endif
