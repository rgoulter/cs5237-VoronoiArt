#ifndef DELAUNAY_DELAUNAYH
#define DELAUNAY_DELAUNAYH

#include <vector>

#include "delaunay/directedgraph.h"
#include "delaunay/li.h"
#include "delaunay/lmath.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"



namespace delaunay {

// POLYREP:POINTSETARRAY
/// Construct Voronoi polygons for a the triangulation of a DirectedGraph which
/// (after Delaunay Triangulation generated for it).
std::vector<PointSetArray> createVoronoi(const DirectedGraph& dag);



/// Run the delaunay triangulation on a Directed Graph.
void runDelaunayTriangulationOn(DirectedGraph&);



// POLYREP:POINTSETARRAY
/// Returns set of Voronoi polygons.
std::vector<PointSetArray> runDelaunayAlgorithm(const PointSetArray& inputPoints);

}

#endif
