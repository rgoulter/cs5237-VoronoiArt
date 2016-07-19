#ifndef GEOMETRY_LINESEGMENTH
#define GEOMETRY_LINESEGMENTH

#include <utility>

#include "point.h"

namespace geometry {

/// Abstraction for referring to (directed) line segments
template<typename I>
using LineSegment = std::pair<Point<I>, Point<I>>;



enum class Orientation { CounterClockwise = 1,
                         Colinear  = 0,
                         Clockwise = -1 };

/// Returns 1 if p3 is counter-clockwise to p1p2,
/// 0 if co-linear,
/// -1 if clockwise.
// TODO enum class for type safety
template<typename I>
int orientation(const LineSegment<I>& p1p2, const Point<I>& p3);


// ColinearOverlap only used in intersects(LineSeg,LineSeg),
// so perhaps should be different type
enum class Intersection { None, Incidental, ColinearOverlap, Overlap };

template<typename I>
Intersection intersects(const LineSegment<I>& ab, const LineSegment<I>& cd);

/// Whether the two segments Overlap. (i.e. Incidental returns false).
bool isOverlapping(const Intersection& i);

/// Whether the two segments touch. (Incidental or Overlap)
bool isTouching(const Intersection& i);



/// IMPRECISE
/// Return approximate intersection point of line segments ab, cd.
/// Undefined Behaviour if LineSeg ab doesn't intersect w/ cd.
Point<int> findIntersectionPoint(const LineSegment<int>& ab, const LineSegment<int>& cd);

}

#endif
