#ifndef POINTSETARRAYH
#define POINTSETARRAYH

#include "pointSet.h"


class PointSetArray : public PointSet {

public:

	int getPoint (int pIndex, LongInt& x1,LongInt& y1); // put the x,y values into x1,y1, and return 1 if the point pIndex exists
	int noPt();                                         // return the number of points
	void eraseAllPoints();                              // erase all points in this array

};

std::vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa);

#endif