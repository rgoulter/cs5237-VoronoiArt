#ifndef DELAUNAY_DELAUNAYTRIH
#define DELAUNAY_DELAUNAYTRIH

#include "delaunay/directedgraph.h"



namespace delaunay {

// XXX WTF is this even doing? Impl. in delaunay.cpp
class DelaunayTri {
public:
	// Method to search the DAG for the triangle containing the point. This triangle will be subdivided into smaller triangles.
	//bool isLocallyDelaunay(int , int );

	static void legalizeEdge(DirectedGraph&, int, int, int);
};

}

#endif
