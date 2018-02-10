#ifndef DELAUNAY_TRIANGLEGEOMETRYH
#define DELAUNAY_TRIANGLEGEOMETRYH

#include <utility>

#include "delaunay/longint/li.h"

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#include "geometry/linesegment.h"



namespace delaunay {

/// Method to add new children triangles to a parent triangle.
/// Call this from tryInsertPoint method in main.cpp
void findBoundingTri(PointSetArray<LongInt>&);



int inTriangle(const PointSetArray<LongInt>& psa, const TriRecord& tri, int pIdx);



/// positive-y points UP.
bool isTriangleCCW(const PointSetArray<LongInt>& psa, const TriRecord& tri);



geometry::Intersection intersectsTriangle(const PointSetArray<LongInt>& psa, const TriRecord& tri, std::pair<int,int> idxPt);



geometry::Intersection intersectsTriangle(const PointSetArray<LongInt>& psa, const TriRecord& tri1, const TriRecord& tri2);

}

#endif
