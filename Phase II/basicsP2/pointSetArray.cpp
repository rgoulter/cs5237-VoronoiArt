#include "pointSetArray.h"



int PointSetArray::getPoint (int obpIndex, LongInt& x1,LongInt& y1) {
	// one-based point index
	if(0 < obpIndex && obpIndex <= myPoints.size()){
		MyPoint *p = &(myPoints[obpIndex - 1]);
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