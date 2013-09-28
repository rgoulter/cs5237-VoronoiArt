#ifndef DIRECTEDGRAPHH
#define DIRECTEDGRAPHH

#include <map>  
#include <set>
#include <utility>

#include "pointSet.h"
#include "pointSetArray.h"
#include "trist.h"

/* 
*/

class SomeName {

	public:
		// Method to add new children triangles to a parent triangle. Call this from tryInsertPoint method in main.cpp
		void findBoundingTri(PointSet );
		
		// Method to search the DAG for the triangle containing the point. This triangle will be subdivided into smaller triangles.
		bool isLocallyDelaunay(int , int , PointSet);
		
		// Returns a set of triangles for the input point indexes.
		std::vector<TriRecord> findNodesforEdge(int, int );

	protected:
		// Keeps the relationship between a parent node and its children.
		std::map<TriRecord, std::vector<TriRecord> > dagNode;

};

#endif