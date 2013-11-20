#ifndef CS5237_POLGYONH
#define CS5237_POLGYONH

#include <vector>

class MyPoint {
public:
	LongInt x;
	LongInt y;

	MyPoint();
	MyPoint(LongInt x, LongInt y);

	bool operator==(const MyPoint& otherPoint) {
		return x == otherPoint.x && y == otherPoint.y;
	}
};

int orientation(const MyPoint& p1, const MyPoint& p2, const MyPoint& p3);

bool intersects(const MyPoint& a, const MyPoint& b, const MyPoint& c, const MyPoint& d);

bool intersects(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy);

void findIntersectionPoint(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy, int& ix, int& iy);

void boundingBox(const std::vector<int>& poly, int& minX, int& maxX, int& minY, int& maxY);

std::vector<int> clipPolygonToRectangle(const std::vector<int>& poly, int x1, int y1, int x2, int y2);

int inPoly(const std::vector<int>& poly, int x, int y);
int inPoly(const std::vector<MyPoint>& poly, const MyPoint & p);


#endif