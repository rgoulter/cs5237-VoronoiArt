#include "geometry/linesegment.h"

#include <algorithm>

#include "delaunay/longint/li.h"

using std::pair;
using std::min;
using std::max;



namespace geometry {

// Adapted from basics/lmath.cpp
template<typename I>
int signDet(const I& x1, const I& y1, const I& w1,
            const I& x2, const I& y2, const I& w2,
            const I& x3, const I& y3, const I& w3) {
	const I& det = x1 * (y2 * w3 - y3 * w2) -
	          x2 * (y1 * w3 - y3 * w1) +
	          x3 * (y1 * w2 - y2 * w1);

	if (det > 0)
		return 1;
	if (det < 0)
		return -1;
	return 0;
}



/// ASSUMPTION that <I> can be constructed from `int`.
template<typename I>
int orientation(const LineSegment<I>& p1p2, const Point<I>& p3) {
	const Point<I>& p1 = p1p2.first;
	const Point<I>& p2 = p1p2.second;
	return signDet(p1.x, p1.y, I(1),
	               p2.x, p2.y, I(1),
	               p3.x, p3.y, I(1));
}

// Instantiate for LongInt
template int orientation(const LineSegment<int>&, const Point<int>&);
template int orientation(const LineSegment<double>&, const Point<double>&);
template int orientation(const LineSegment<delaunay::LongInt>&, const Point<delaunay::LongInt>&);



template<typename I>
Intersection intersects1D(const pair<I,I>& ab, const pair<I,I>& cd) {
	const I& a = min(ab.first, ab.second);
	const I& b = max(ab.first, ab.second);
	const I& c = min(cd.first, cd.second);
	const I& d = max(cd.first, cd.second);

	if ((b < c) || (d < a)) {
		return Intersection::None;
	} else if ((b == c && a < b) ||
	           (d == a && c < d)) {
		return Intersection::Incidental;
	} else {
		return Intersection::Overlap;
	}
}

template Intersection intersects1D(const pair<int, int>&, const pair<int,int>&);



template<typename I>
Intersection intersects(const LineSegment<I>& ab, const LineSegment<I>& cd) {
	const Point<I>& a = ab.first;
	const Point<I>& b = ab.second;
	const Point<I>& c = cd.first;
	const Point<I>& d = cd.second;

	// For int val, of orient(pq, r) * orient(pq, s):
	//   -1 = isect (one CCW, one CW),
	//    0 = touching (one colinear)
	//    1 = not touching (both CCW OR both CW)
	int abcOri = orientation({a, b}, c);
	int abdOri = orientation({a, b}, d);
	int abIsect = abcOri * abdOri;
	int cdaOri = orientation({c, d}, a);
	int cdbOri = orientation({c, d}, b);
	int cdIsect = cdaOri * cdbOri;

	if (abIsect > 0 || cdIsect > 0) {
		return Intersection::None;
	} else if (abIsect == 0 || cdIsect == 0) {
		if (abcOri == 0 && abdOri == 0 &&
		    cdaOri == 0 && cdbOri == 0) {
		    // Special case: All Colinear to each other.
		    // Check x-projection && y-projection
		    Intersection xIsect = intersects1D<I>({a.x, b.x}, {c.x, d.x});
		    Intersection yIsect = intersects1D<I>({a.y, b.y}, {c.y, d.y});

			if (xIsect == Intersection::None ||
			    yIsect == Intersection::None) {
			    return Intersection::None;
			} else if (xIsect == Intersection::Incidental ||
			           yIsect == Intersection::Incidental) {
				return Intersection::Incidental;
			} else {
				return Intersection::ColinearOverlap;
			}
		} else {
			// abIsect <= 0, cdIsect <= 0
			return Intersection::Incidental;
		}
	} else {
		// abIsect <0 && cdIsect < 0
		return Intersection::Overlap;
	}
}

// Instantiate for int, LongInt
template Intersection intersects(const LineSegment<int>&, const LineSegment<int>&);
template Intersection intersects(const LineSegment<double>&, const LineSegment<double>&);
template Intersection intersects(const LineSegment<delaunay::LongInt>&, const LineSegment<delaunay::LongInt>&);



bool isOverlapping(const Intersection& i) {
	return i == Intersection::Overlap;
}



bool isTouching(const Intersection& i) {
	// touching = Incidental || Overlap
	return i != Intersection::None;
}



int crossProduct2D(const Point<int>& v, const Point<int>& w) {
	// cross-product of 2D V,W is vx*wy - vy*wx
	return (v.x * w.y) - (v.y * w.x);
}



/// Assumes the points actually intersect.
Point<int> findIntersectionPoint(const LineSegment<int>& ab, const LineSegment<int>& cd) {
	int ax = ab.first.x;
	int ay = ab.first.y;
	int bx = ab.second.x;
	int by = ab.second.y;
	int cx = cd.first.x;
	int cy = cd.first.y;
	int dx = cd.second.x;
	int dy = cd.second.y;

	// Logic from StackOverflow, Gareth Rees's answer.
	// See: http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

	// t = cp((C-A), S) / cp(R, S)
	// where R = (B - A),
	//       S = (D - C)

	// TODO: Code would be simpler if Point<I> had operator-
	int rx = bx - ax;
	int ry = by - ay;
	int sx = dx - cx;
	int sy = dy - cy;

	float t = float(crossProduct2D({cx - ax, cy - ay}, {sx, sy})) /
	                crossProduct2D({rx, ry}, {sx, sy});

	// TODO This needs clarification; or use `u = 1 - t` instead.
	// with our definitions, we want to use (1 - t), actually.
	t = 1 - t;

	// A * t + (1 - t) * B
	return Point<int>((int) (ax * t + (1 - t) * bx),
	                  (int) (ay * t + (1 - t) * by));
}

}
