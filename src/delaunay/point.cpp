#include "point.h"



MyPoint::MyPoint() {
	this->x = 0;
	this->y = 0;
}



MyPoint::MyPoint(LongInt x, LongInt y) {
	this->x = x;
	this->y = y;
}



std::ostream& operator<<(std::ostream& os, const MyPoint& pt) {
	os << "(" << pt.x << "," << pt.y << ")";
	return os;
}

