#ifndef GEOMETRY_POINTH
#define GEOMETRY_POINTH

#include <ostream>
#include <utility>



namespace geometry {

template<typename I>
class Point {
public:
	I x;
	I y;

	Point() : x(), y() {}
	Point(const I& nx, const I& ny) : x(nx), y(ny) {}
	Point(const std::pair<I,I>& p) : x(p.first), y(p.second) {}

	bool operator==(const Point<I>& otherPoint) const {
		return x == otherPoint.x && y == otherPoint.y;
	}

	template<typename J>
	friend std::ostream& operator<<(std::ostream& os, const Point<J>& pt);
};



template <typename I>
std::ostream& operator<<(std::ostream& os, const Point<I>& pt) {
	os << "(" << pt.x << "," << pt.y << ")";
	return os;
}


}

#endif
