#ifndef DELAUNAY_DELAUNAYH
#define DELAUNAY_DELAUNAYH

#include <vector>


#include "delaunay/longint/li.h"
#include "delaunay/longint/lmath.h"

#include "delaunay/directedgraph.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#include "geometry/polygon.h"



namespace delaunay {

/// Construct Voronoi polygons for a the triangulation of a DirectedGraph which
/// (after Delaunay Triangulation generated for it).
std::vector<geometry::Polygon> createVoronoi(const DirectedGraph& dag);



/// Run the delaunay triangulation on a Directed Graph.
void runDelaunayTriangulationOn(DirectedGraph&);



// POLYREP:POINTSETARRAY
/// Returns set of Voronoi polygons.
std::vector<geometry::Polygon> runDelaunayAlgorithm(const PointSetArray& inputPoints);

}

#endif
