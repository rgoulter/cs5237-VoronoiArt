#ifndef DELAUNAYH
#define DELAUNAYH

#include "pointset.h"
#include "pointsetarray.h"
#include "trist.h"
#include "directedgraph.h"



// XXX WTF is this even doing? Impl. in delaunay.cpp
class DelaunayTri {
public:
	// Method to search the DAG for the triangle containing the point. This triangle will be subdivided into smaller triangles.
	//bool isLocallyDelaunay(int , int );

	static void legalizeEdge(DirectedGraph&, int, int, int);
};

#endif
