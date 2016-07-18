#ifndef GEOMETRY_POLYGONH
#define GEOMETRY_POLYGONH

#include <assert.h>

#include <utility>
#include <vector>

#include "geometry/linesegment.h"
#include "geometry/point.h"

namespace geometry {

using Edge = LineSegment<int>;



class Polygon {
public:
	Polygon() {}

	void addPoint(int x, int y) { points_.push_back(Point<int>(x,y)); }

	void addPoint(const Point<int>& pt) { points_.push_back(pt); }

	int numPoints() const { return points_.size(); }

	/// Get a copy of the vector's points.
	std::vector<Point<int>> points() const;

	/// Get a copy of the vector's edges.
	std::vector<Edge> edges() const;

	Point<int> operator[](int idx) const {
		assert(0 <= idx && (unsigned) idx < points_.size());
		return points_[idx];
	}

private:
	std::vector<Point<int>> points_;
};



class Rect {
public:
	// p2.y > p1.y (true whether y+ is Up or Down)
	Rect(Point<int> p1, Point<int> p2) :
		x_(p1.x), y_(p1.y),
		width_(p2.x - p1.x), height_(p2.y - p1.y) {}
	Rect(Point<int> p1, int width, int height) :
		x_(p1.x), y_(p1.y),
		width_(width), height_(height) {}

	// XXX which of these do we actually use?
	int left() const { return x_; }
	int right() const { return x_ + width_; }
	// COORD: TOP = y1, BOTTOM = y2
	int top() const { return y_; }
	int bottom() const { return y_ + height_; }

	int x() const { return x_; }
	int y() const { return y_; }
	int width() const { return width_; }
	int height() const { return height_; }

private:
	int x_;
	int y_;
	int width_;
	int height_;
};



Rect boundingBox(const Polygon& poly);



Polygon clipPolygonToRectangle(const Polygon& poly, const Rect& rect);



// TODO { OUTSIDE, TOUCHING, INSIDE } enum class
int inPoly(const Polygon& poly, const Point<int>& pt);

}

#endif
