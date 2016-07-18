#ifndef GEOMETRY_LINESEGMENTH
#define GEOMETRY_LINESEGMENTH

#include <utility>

#include "point.h"

namespace geometry {

/// Abstraction for referring to (directed) line segments
template<typename I>
using LineSegment = std::pair<Point<I>, Point<I>>;



/// Returns 1 if p3 is counter-clockwise to p1p2,
/// 0 if co-linear,
/// -1 if clockwise.
// TODO enum class for type safety
template<typename I>
int orientation(const LineSegment<I>& p1p2, const Point<I>& p3);



// TODO return { NO, TOUCH, INTERSECT } rather than bool. (then replace isects)
bool intersects(const LineSegment<int>& ab, const LineSegment<int>& cd);



/// IMPRECISE
/// Return approximate intersection point of line segments ab, cd.
/// Undefined Behaviour if LineSeg ab doesn't intersect w/ cd.
Point<int> findIntersectionPoint(const LineSegment<int>& ab, const LineSegment<int>& cd);

}

#endif
