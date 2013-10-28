#ifndef CS5237_POLGYONH
#define CS5237_POLGYONH

#include <vector>

class MyPoint {
public:
	LongInt x;
	LongInt y;

	MyPoint();
	MyPoint(LongInt x, LongInt y);

	bool operator==(MyPoint& otherPoint){
		return x==otherPoint.x && y==otherPoint.y;

	}
	/*MyPoint operator+(MyPoint& otherPoint) {
		MyPoint result;
		result.x = x + otherPoint.x;
		result.y = y + otherPoint.y;
		return result;
	}

	MyPoint operator-(MyPoint& otherPoint) {
		MyPoint result;
		result.x = x - otherPoint.x;
		result.y = y - otherPoint.y;
		return result;
	}*/
};

int orientation(const MyPoint& p1, const MyPoint& p2, const MyPoint& p3);

bool intersects(const MyPoint& a, const MyPoint& b, const MyPoint& c, const MyPoint& d);

int inPoly(const std::vector<int>& poly, int x, int y);
int inPoly(const std::vector<MyPoint>& poly, const MyPoint & p);


#endif