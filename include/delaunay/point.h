#ifndef DELAUNAY_POINTH
#define DELAUNAY_POINTH

#include <ostream>

#include "delaunay/li.h"



namespace delaunay {

class MyPoint {
public:
	LongInt x;
	LongInt y;

	MyPoint();
	MyPoint(LongInt x, LongInt y);

	bool operator==(const MyPoint& otherPoint) {
		return x == otherPoint.x && y == otherPoint.y;
	}

	friend std::ostream& operator<<(std::ostream& os, const MyPoint& pt);
};


}

#endif
