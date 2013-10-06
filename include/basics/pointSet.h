#ifndef POINTSETH
#define POINTSETH

#include <vector>
#include "li.h"
#include "lmath.h"

class MyPoint {
public:
	LongInt x;
	LongInt y;
	LongInt z;

	bool operator==(MyPoint& otherPoint){
		return x==otherPoint.x && y==otherPoint.y && z==otherPoint.z;

	}
	/*MyPoint operator+(MyPoint& otherPoint) {
		MyPoint result;
		result.x = x + otherPoint.x;
		result.y = y + otherPoint.y;
		result.z = z + otherPoint.z;
		return result;
	}

	MyPoint operator-(MyPoint& otherPoint) {
		MyPoint result;
		result.x = x - otherPoint.x;
		result.y = y - otherPoint.y;
		result.z = z - otherPoint.z;
		return result;
	}*/
};

class PointSet {
protected:

public:
	std::vector<MyPoint> myPoints;
	// previously was protected. changed because it's needed for findBoundingTri() in main

	int addPoint(LongInt x1,LongInt y1); 
	// add an ith(according to the adding sequence)  point (x1,y1), return i.

	void deleteLastPoint();

	int inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx);
	// return 1 if the point p is inside the triangle p1p2p3, by reference of the indices, -1 if outside, 0 if degenerate

	int inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx);
	// return 1 if the point p is inside the circumcircle of p1p2p3, -1 if outside, 0 if degenerate

};

#endif