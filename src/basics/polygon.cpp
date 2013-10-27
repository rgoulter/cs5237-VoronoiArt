#include "lmath.h"
#include "polygon.h"
#include <vector>

MyPoint::MyPoint() {
	this->x = 0;
	this->y = 0;
}

MyPoint::MyPoint(LongInt x, LongInt y) {
	this->x = x;
	this->y = y;
}

int orientation(const MyPoint& p1, const MyPoint& p2, const MyPoint& p3) {
	return signDet(p1.x, p1.y, LongInt(1),
				   p2.x, p2.y, LongInt(1),
				   p3.x, p3.y, LongInt(1));
}

bool intersects(const MyPoint& a, const MyPoint& b, const MyPoint& c, const MyPoint& d) {
	return (orientation(a, b, c) * orientation(a, b, d) <= 0) &&
		   (orientation(c, d, a) * orientation(c, d, b) <= 0);
}

int inPoly(const std::vector<MyPoint>& poly, const MyPoint & pt) {
	int n = poly.size();

	// Find some point outsize of the poly.
	int smallestXPtIdx = 0;
	
	for (int i = 1; i < n; i++) {
		if (poly[i].x < poly[smallestXPtIdx].x) {
			smallestXPtIdx = i;
		}
	}

	MyPoint outsidePt;
	outsidePt.x = poly[smallestXPtIdx].x - 1; // must be outside of the polygon.
	outsidePt.y = poly[smallestXPtIdx].y;

	// Now count intersections
	int numIntersections = 0;

	for (int i = 0; i < n; i++) {
		if (intersects(outsidePt, pt, poly[i], poly[(i + 1) % n])) {
			numIntersections++;
		}
	}

	return (numIntersections % 2) == 1;
}