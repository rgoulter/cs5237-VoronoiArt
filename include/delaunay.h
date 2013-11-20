#ifndef CS5237_DELAUNAYH
#define CS5237_DELAUNAYH

#include "li.h"
#include "lmath.h"
#include "pointSet.h"

#include "pointSetArray.h"
#include "trist.h"

#include "delaunayTri.h"
#include "directedGraph.h"


void delaunayIterationStep();



// This method checks whether the voronoi edge identified already exists in the existing voronoi edge set.
bool checkedgeExists(PointSetArray voronoiEdge);



void createVoronoi();




// Call this function when the user pushes the button to do Delaunay Triangulation
void tryDelaunayTriangulation();

#endif