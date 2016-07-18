#ifndef DELAUNAY_POLGYONH
#define DELAUNAY_POLGYONH

#include <vector>

#include "delaunay/li.h"

#include "geometry/point.h"



namespace delaunay {

/// Refactoring MyPoint to a general template Point<I>;
/// this alias serves as an intermediate.
using MyPoint = geometry::Point<LongInt>;

}

#endif
