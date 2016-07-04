#ifndef POINTSETARRAYH
#define POINTSETARRAYH

#include "pointSet.h"



class PointSetArray : public PointSet {
public:
	/// put the x,y values into x1,y1, and return 1 if the point pIndex exists
	int getPoint (int pIndex, LongInt& x1,LongInt& y1) const;
	int noPt() const;                                         // return the number of points
	void eraseAllPoints();                              // erase all points in this array
};

std::vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa);

#endif
