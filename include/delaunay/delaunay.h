#ifndef DELAUNAY_DELAUNAYH
#define DELAUNAY_DELAUNAYH

#include <vector>

#include "delaunay/directedgraph.h"
#include "delaunay/li.h"
#include "delaunay/lmath.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"



namespace delaunay {

// This method checks whether the voronoi edge identified already exists in the existing voronoi edge set.
// bool checkedgeExists(PointSetArray voronoiEdge);



std::vector<PointSetArray> createVoronoi(DirectedGraph& dag);



// Call this function when the user pushes the button to do Delaunay Triangulation
void tryDelaunayTriangulation(DirectedGraph&);



void legalizeEdge(DirectedGraph&, int, int, int);

}

#endif
