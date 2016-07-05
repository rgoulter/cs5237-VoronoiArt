#ifndef DIRECTEDGRAPHH
#define DIRECTEDGRAPHH

#include <assert.h>

#include <iostream>
#include <map>
#include <set>

#include "pointset.h"
#include "pointsetarray.h"
#include "trist.h"



/* This Directed acyclic graph maintains a point location structure, which helps us find out the triangle in which
the current point under consideration is.

The leaf nodes of this graph gives the triangles in the current state of triangulation. The top node is the
bounding triangle of all the points in the delaunay triangulation.

Whenever a new point is added and triangulated, this graph should get children nodes from the parent triangle that
got deleted.
*/


class DirectedGraph {
public:
	DirectedGraph(PointSetArray);

	/// Finds the linked delaunay triangles for the input point index
	std::vector<TriRecord> findLinkedNodes(int);

	/// Returns a set of triangles for the input point indexes.
	std::vector<TriRecord> findNodesForEdge(int, int);

	/// Method to search the DAG for the triangle containing the point.
	/// This triangle will be subdivided into smaller triangles.
	TriRecord findLeafNodeForPoint(int);

	std::vector<TriRecord> getLeafNodes();

	/// Method to add new children triangles to a parent triangle.
	/// Use findLeafNodeforPoint to find parent,
	/// and then create child node for this triangle.
	void addChildrenNodes(int);

	/// Create children nodes with 2 parents in case of edge flipping.
	void addFlipChildrenNodes(int, int, int, int);

	/// Resets the graph, removes everything from it.
	void clearDirectedGraph();

	const PointSetArray& getPointSet() const {
		return triVertices_;
	}

protected:
	// Keeps the relationship between a parent node and its children.
	std::map<TriRecord, std::vector<TriRecord> > dagNode_;

	PointSetArray triVertices_;

private:
	//  Remove if not needed.
	/// Vector that keeps the order in which keys got inserted into DAG.
	std::vector<TriRecord> orderedKeyList_;
	std::vector<TriRecord> leafNodeList_;
};

#endif
