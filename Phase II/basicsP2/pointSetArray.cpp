#include "pointSetArray.h"



int PointSetArray::getPoint (int pIndex, LongInt& x1,LongInt& y1) {
	if(0 <= pIndex && pIndex <= myPoints.size()){
		MyPoint *p = &(myPoints[pIndex]);
		x1 = p->x;
		y1 = p->y;

		return 1;
	}
	return 0;
}



int PointSetArray::noPt () {
	return myPoints.size();
}



void PointSetArray::eraseAllPoints () {
	// Do we need to be careful with memory here?
	myPoints.clear();
}