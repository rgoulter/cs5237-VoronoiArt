#ifndef CS5237_POLGYONH
#define CS5237_POLGYONH

#include <vector>

class MyPoint {
public:
	LongInt x;
	LongInt y;

	MyPoint();
	MyPoint(LongInt x, LongInt y);

	bool operator==(const MyPoint& otherPoint){
		return x == otherPoint.x && y == otherPoint.y;

	}
};

int orientation(const MyPoint& p1, const MyPoint& p2, const MyPoint& p3);

bool intersects(const MyPoint& a, const MyPoint& b, const MyPoint& c, const MyPoint& d);

void boundingBox(const std::vector<int>& poly, int& minX, int& maxX, int& minY, int& maxY);

int inPoly(const std::vector<int>& poly, int x, int y);
int inPoly(const std::vector<MyPoint>& poly, const MyPoint & p);


#endif