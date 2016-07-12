#include "pointsetarray.h"

#include <assert.h>

#include <vector>

using std::vector;



int PointSetArray::getPoint(int obpIndex, LongInt& x1, LongInt& y1) const {
	// one-based point index
	if (0 < obpIndex && obpIndex <= int(myPoints.size())) {
		const MyPoint& p = myPoints[obpIndex - 1];
		x1 = p.x;
		y1 = p.y;

		return 1;
	}

	return 0;
}



int PointSetArray::noPt() const {
	return myPoints.size();
}



void PointSetArray::eraseAllPoints() {
	// Do we need to be careful with memory here?
	myPoints.clear();
}



vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa) {
	vector<int> poly;

	// Coerce psa to poly<int>
	for (int ptIdx = 1; ptIdx <= psa.noPt(); ptIdx++) {
		LongInt x, y;
		psa.getPoint(ptIdx, x, y);
		poly.push_back((int) x.doubleValue());
		poly.push_back((int) y.doubleValue()); 
	}

	return poly;
}



const MyPoint& PointSetArray::operator[](int idx) const {
	assert(1 <= idx && idx <= noPt());

	return myPoints[idx - 1];
}

