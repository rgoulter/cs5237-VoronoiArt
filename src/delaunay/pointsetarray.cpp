#include "delaunay/pointsetarray.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include "geometry/linesegment.h"

using std::vector;

using geometry::orientation;



namespace delaunay {

int PointSetArray::addPoint(LongInt x1, LongInt y1) {
	MyPoint thisPoint(x1, y1);

	myPoints.push_back(thisPoint);
	return (myPoints.size());
}



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



//this function gives an incorrect response for 1-2 cases per trial.
//The only possiblity is my orientation function or whatever it depends on has a bug.
//mainly because this code is stupidly simple.
int PointSetArray::inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx) const {
	//If v is in the triangle formed by (p1, p2), (p2, p3), (p3, p1)
	//then v is consistently on the same side of all of those lines.

	// Indices given are 1-based, but stored as 0-based.
	MyPoint p1 = myPoints[p1Idx - 1];
	MyPoint p2 = myPoints[p2Idx - 1];
	MyPoint p3 = myPoints[p3Idx - 1];
	MyPoint v = myPoints[pIdx - 1]; // the other point.

	//remove cases where any 2 points are equal.
	if (p1 == p2 || p2 == p3 || p3 == p1)
		return 0;

	int orientation1, orientation2, orientation3;
	//p1->p2
	orientation1 = orientation({p1, p2}, v);
	//p2->p3
	orientation2 = orientation({p2, p3}, v);
	//p3->p1
	orientation3 = orientation({p3, p1}, v);
	//std::cout<<orientation1<<", "<<orientation2<<","<<orientation3<<std::endl;
	//if v is on any of the lines, we return 0.
	if (orientation1 == 0 || orientation2 == 0 || orientation3 == 0)
		return 0;
	else if (orientation1 == orientation2 && orientation2 == orientation3)
		return 1;
	else
		return -1;
}



/// assume that p1,p2,p3 must be CCW
int PointSetArray::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) const {
	// std::cout << "inCircle " << p1Idx << "," << p2Idx << "," << p3Idx << ", " << pIdx << "; len=" << (myPoints.size()) << std::endl;

	// Indices given are 1-based, but stored as 0-based.
	MyPoint p1 = myPoints[p1Idx - 1];
	MyPoint p2 = myPoints[p2Idx - 1];
	MyPoint p3 = myPoints[p3Idx - 1];
	MyPoint v = myPoints[pIdx - 1]; // the other point.
	//if any 2 points are the same, return 0 immediately.
	if (p1 == p2 || p2 == p3 || p1 == p3)
		return 0;
	int orient = orientation({p1, p2}, p3);
	if (orient == 0) //if they are colinear.
		return 1;
	//look here for formula: http://www.cs.cmu.edu/~quake/robust.html
	LongInt t1x, t1y, t2x, t2y, t3x, t3y;
	t1x = p1.x - v.x;
	t1y = p1.y - v.y;
	t2x = p2.x - v.x;
	t2y = p2.y - v.y;
	t3x = p3.x - v.x;
	t3y = p3.y - v.y;

	// this is a refactoring of the lecturer's solution to be a 3x3 determinant instead.
	return signDet(t1x, t1y, t1x * (p1.x + v.x) + t1y * (p1.y + v.y),
	               t2x, t2y, t2x * (p2.x + v.x) + t2y * (p2.y + v.y),
	               t3x, t3y, t3x * (p3.x + v.x) + t3y * (p3.y + v.y)) * orient;
}



// This method returns the circumcircle center for the three input points.
bool PointSetArray::circumCircle(int p1Idx, int p2Idx, int p3Idx, MyPoint& center) const {
	LongInt dA, dB, dC, aux1, aux2, div;
	int quo1, quo2; // aux1Sign, aux2Sign;
	LongInt two = 2;
	// std::cout << "MyPoints size " << myPoints.size() << std::endl;
	// std::cout << "accessing: " << (p1Idx - 1) << ", " << (p2Idx - 1) << ", " << (p3Idx - 1) << std::endl;
	// std::cout << "circumCirle 1.1" << std::endl;
	MyPoint p1 = myPoints[p1Idx - 1]; // ... SIGSEGV here?
	// std::cout << "circumCirle 1.2" << std::endl;
	MyPoint p2 = myPoints[p2Idx - 1];
	// std::cout << "circumCirle 1.3" << std::endl;
	MyPoint p3 = myPoints[p3Idx - 1];
	// std::cout << "circumCirle 1.4" << std::endl;

	dA = p1.x * p1.x + p1.y * p1.y;
	dB = p2.x * p2.x + p2.y * p2.y;
	dC = p3.x * p3.x + p3.y * p3.y;

	aux1 = (dA*(p3.y - p2.y) + dB*(p1.y - p3.y) + dC*(p2.y - p1.y));
	aux2 = LongInt(0) - (dA*(p3.x - p2.x) + dB*(p1.x - p3.x) + dC*(p2.x - p1.x)); // Negate not actually defined
	div = (two*(p1.x*(p3.y - p2.y) + p2.x*(p1.y-p3.y) + p3.x*(p2.y - p1.y)));

	// std::cout << "circumCirle 2" << std::endl;

	if (div == 0) {
		return false;
	}

	LongInt aux1sign = aux1.sign();
	LongInt aux2sign = aux2.sign();
	LongInt divsign = div.sign();
	LongInt aux1modval = aux1*aux1sign;
	LongInt aux2modval = aux2*aux2sign;
	LongInt divmodval = div*divsign;

	// Repeated subtraction for division
	for (quo1 = 0; aux1modval - divmodval >= 0; quo1++) {
		aux1modval = aux1modval - divmodval;
	}

	for (quo2 = 0; aux2modval - divmodval >= 0; quo2++) {
		aux2modval = aux2modval - divmodval;
	}

	center.x = ((LongInt)quo1)*aux1sign*divsign;
	center.y = ((LongInt)quo2)*aux2sign*divsign;

	return true;
}



void PointSetArray::eraseAllPoints() {
	// Do we need to be careful with memory here?
	myPoints.clear();
}



const MyPoint& PointSetArray::operator[](int idx) const {
	assert(1 <= idx && idx <= noPt());

	return myPoints[idx - 1];
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

}

