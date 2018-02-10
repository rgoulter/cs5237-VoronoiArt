#ifndef DELAUNAY_DELAUNAYH
#define DELAUNAY_DELAUNAYH

#include <vector>



#include "delaunay/directedgraph.h"
#include "delaunay/pointsetarray.h"

#include "geometry/polygon.h"



namespace delaunay {

/// Construct Voronoi polygons for a the triangulation of a DirectedGraph which
/// (after Delaunay Triangulation generated for it).
template<typename I>
std::vector<geometry::Polygon> createVoronoi(const DirectedGraph<I>& dag);



/// Run the delaunay triangulation on a Directed Graph.
template<typename I>
void runDelaunayTriangulationOn(DirectedGraph<I>&);



// POLYREP:POINTSETARRAY
/// Returns set of Voronoi polygons.
template<typename I>
std::vector<geometry::Polygon> runDelaunayAlgorithm(const PointSetArray<I>& inputPoints);

}

#endif
