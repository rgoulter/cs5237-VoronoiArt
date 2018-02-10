#include "delaunay/pointsetarray.h"

#include <assert.h>

#include <iostream>
#include <vector>

#include "delaunay/longint/li.h"
#include "delaunay/longint/lmath.h"

#include "geometry/linesegment.h"

using std::vector;

using geometry::orientation;
using geometry::Point;



namespace delaunay {

template<typename I>
int PointSetArray<I>::addPoint(I x1, I y1) {
	Point<I> thisPoint(x1, y1);

	myPoints.push_back(thisPoint);
	return (myPoints.size());
}



template<typename I>
int PointSetArray<I>::getPoint(int obpIndex, I& x1, I& y1) const {
	// one-based point index
	if (0 < obpIndex && obpIndex <= int(myPoints.size())) {
		const Point<I>& p = myPoints[obpIndex - 1];
		x1 = p.x;
		y1 = p.y;

		return 1;
	}

	return 0;
}



template<typename I>
int PointSetArray<I>::noPt() const {
	return myPoints.size();
}



//this function gives an incorrect response for 1-2 cases per trial.
//The only possiblity is my orientation function or whatever it depends on has a bug.
//mainly because this code is stupidly simple.
template<typename I>
int PointSetArray<I>::inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx) const {
	//If v is in the triangle formed by (p1, p2), (p2, p3), (p3, p1)
	//then v is consistently on the same side of all of those lines.

	// Indices given are 1-based, but stored as 0-based.
	Point<I> p1 = myPoints[p1Idx - 1];
	Point<I> p2 = myPoints[p2Idx - 1];
	Point<I> p3 = myPoints[p3Idx - 1];
	Point<I> v = myPoints[pIdx - 1]; // the other point.

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
template<typename I>
int PointSetArray<I>::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) const {
	// std::cout << "inCircle " << p1Idx << "," << p2Idx << "," << p3Idx << ", " << pIdx << "; len=" << (myPoints.size()) << std::endl;

	// Indices given are 1-based, but stored as 0-based.
	Point<I> p1 = myPoints[p1Idx - 1];
	Point<I> p2 = myPoints[p2Idx - 1];
	Point<I> p3 = myPoints[p3Idx - 1];
	Point<I> v = myPoints[pIdx - 1]; // the other point.
	//if any 2 points are the same, return 0 immediately.
	if (p1 == p2 || p2 == p3 || p1 == p3)
		return 0;
	int orient = orientation({p1, p2}, p3);
	if (orient == 0) //if they are colinear.
		return 1;
	//look here for formula: http://www.cs.cmu.edu/~quake/robust.html
	I t1x, t1y, t2x, t2y, t3x, t3y;
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
template<typename I>
bool PointSetArray<I>::circumCircle(int p1Idx, int p2Idx, int p3Idx, Point<I>& center) const {
	I dA, dB, dC, aux1, aux2, div;
	int quo1, quo2; // aux1Sign, aux2Sign;
	I two = 2;
	// std::cout << "MyPoints size " << myPoints.size() << std::endl;
	// std::cout << "accessing: " << (p1Idx - 1) << ", " << (p2Idx - 1) << ", " << (p3Idx - 1) << std::endl;
	// std::cout << "circumCirle 1.1" << std::endl;
	Point<I> p1 = myPoints[p1Idx - 1]; // ... SIGSEGV here?
	// std::cout << "circumCirle 1.2" << std::endl;
	Point<I> p2 = myPoints[p2Idx - 1];
	// std::cout << "circumCirle 1.3" << std::endl;
	Point<I> p3 = myPoints[p3Idx - 1];
	// std::cout << "circumCirle 1.4" << std::endl;

	dA = p1.x * p1.x + p1.y * p1.y;
	dB = p2.x * p2.x + p2.y * p2.y;
	dC = p3.x * p3.x + p3.y * p3.y;

	aux1 = (dA*(p3.y - p2.y) + dB*(p1.y - p3.y) + dC*(p2.y - p1.y));
	aux2 = I(0) - (dA*(p3.x - p2.x) + dB*(p1.x - p3.x) + dC*(p2.x - p1.x)); // Negate not actually defined
	div = (two*(p1.x*(p3.y - p2.y) + p2.x*(p1.y-p3.y) + p3.x*(p2.y - p1.y)));

	// std::cout << "circumCirle 2" << std::endl;

	if (div == 0) {
		return false;
	}

	I aux1sign = aux1.sign();
	I aux2sign = aux2.sign();
	I divsign = div.sign();
	I aux1modval = aux1*aux1sign;
	I aux2modval = aux2*aux2sign;
	I divmodval = div*divsign;

	// Repeated subtraction for division
	for (quo1 = 0; aux1modval - divmodval >= 0; quo1++) {
		aux1modval = aux1modval - divmodval;
	}

	for (quo2 = 0; aux2modval - divmodval >= 0; quo2++) {
		aux2modval = aux2modval - divmodval;
	}

	center.x = ((I)quo1)*aux1sign*divsign;
	center.y = ((I)quo2)*aux2sign*divsign;

	return true;
}



template<typename I>
void PointSetArray<I>::eraseAllPoints() {
	// Do we need to be careful with memory here?
	myPoints.clear();
}



template<typename I>
const Point<I>& PointSetArray<I>::operator[](int idx) const {
	assert(1 <= idx && idx <= noPt());

	return myPoints[idx - 1];
}



template<typename I>
vector<int> coercePSAPolyToIVecPoly(PointSetArray<I>& psa) {
	vector<int> poly;

	// Coerce psa to poly<int>
	for (int ptIdx = 1; ptIdx <= psa.noPt(); ptIdx++) {
		I x, y;
		psa.getPoint(ptIdx, x, y);
		poly.push_back((int) x.doubleValue());
		poly.push_back((int) y.doubleValue()); 
	}

	return poly;
}

template class PointSetArray<LongInt>;

}

