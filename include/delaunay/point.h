#ifndef POINTH
#define POINTH

#include <ostream>

#include "li.h"



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


#endif
