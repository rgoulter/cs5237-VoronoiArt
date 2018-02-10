#ifndef DELAUNAY_TRIANGLEGEOMETRYH
#define DELAUNAY_TRIANGLEGEOMETRYH

#include <utility>

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#include "geometry/linesegment.h"



namespace delaunay {

/// Method to add new children triangles to a parent triangle.
/// Call this from tryInsertPoint method in main.cpp
template<typename I>
void findBoundingTri(PointSetArray<I>&);



template<typename I>
int inTriangle(const PointSetArray<I>& psa, const TriRecord& tri, int pIdx);



/// positive-y points UP.
template<typename I>
bool isTriangleCCW(const PointSetArray<I>& psa, const TriRecord& tri);



template<typename I>
geometry::Intersection intersectsTriangle(const PointSetArray<I>& psa, const TriRecord& tri, std::pair<int,int> idxPt);



template<typename I>
geometry::Intersection intersectsTriangle(const PointSetArray<I>& psa, const TriRecord& tri1, const TriRecord& tri2);

}

#endif
