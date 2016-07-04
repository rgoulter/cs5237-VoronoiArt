#ifndef DELAUNAYH
#define DELAUNAYH

#include "pointSet.h"
#include "pointSetArray.h"
#include "trist.h"
#include "directedGraph.h"



// XXX WTF is this even doing? Impl. in delaunay.cpp
class DelaunayTri {
public:
	// Method to add new children triangles to a parent triangle. Call this from tryInsertPoint method in main.cpp
	static void findBoundingTri(PointSetArray &);

	// Method to search the DAG for the triangle containing the point. This triangle will be subdivided into smaller triangles.
	//bool isLocallyDelaunay(int , int );

	static void legalizeEdge(DirectedGraph&, int, int, int);
};

#endif
