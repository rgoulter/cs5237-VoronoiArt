#ifndef DELAUNAY_TRIANGLEGEOMETRYH
#define DELAUNAY_TRIANGLEGEOMETRYH

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"



namespace delaunay {

/// Method to add new children triangles to a parent triangle.
/// Call this from tryInsertPoint method in main.cpp
void findBoundingTri(PointSetArray &);



int inTriangle(const PointSetArray& psa, const TriRecord& tri, int pIdx);



/// positive-y points UP.
bool isTriangleCCW(const PointSetArray& psa, const TriRecord& tri);



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri, int pIdx1, int pIdx2);



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri1, const TriRecord& tri2);

}

#endif
