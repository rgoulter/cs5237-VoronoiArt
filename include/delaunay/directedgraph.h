#ifndef DIRECTEDGRAPHH
#define DIRECTEDGRAPHH

#include <assert.h>

#include <iostream>
#include <vector>

#include "pointset.h"
#include "pointsetarray.h"
#include "trist.h"



class DAGNode {
public:
	static std::vector<DAGNode*> leafNodesContainingPoint(DAGNode* root, const PointSetArray& pointSet, int pIdx);

	/// 'contains edge' iff 'contains both points'.
	/// Order of points not considered.
	static std::vector<DAGNode*> leafNodesContainingEdge(DAGNode* root, const PointSetArray& pointSet, int pIdx1, int pIdx2);

	DAGNode(const TriRecord& tri) : tri_(tri) {}

	bool isLeaf() const { return children_.empty(); }

	TriRecord tri_;
	std::vector<DAGNode *> children_;
};



/* This Directed acyclic graph maintains a point location structure, which helps us find out the triangle in which
the current point under consideration is.

The leaf nodes of this graph gives the triangles in the current state of triangulation. The top node is the
bounding triangle of all the points in the delaunay triangulation.

Whenever a new point is added and triangulated, this graph should get children nodes from the parent triangle that
got deleted.
*/


class DirectedGraph {
public:
	DirectedGraph(const PointSetArray&);
	~DirectedGraph();

	/// Finds the linked delaunay triangles for the input point index
	std::vector<TriRecord> findTrianglesWithVertex(int) const;

	/// Return a pointIdx for a triangle which shares pIdx2, pIdx3,
	/// but not pIdx1.
	/// Returns `0` if couldn't find an adjacent triangle.
	int findAdjacentTriangle(int pIdx1, int pIdx2, int pIdx3) const;

	/// Method to add new children triangles to a parent triangle.
	TriRecord addVertex(int);

	/// Create children nodes with 2 parents in case of edge flipping.
	void flipTriangles(int, int, int, int);

	const PointSetArray& getPointSet() const {
		return pointSet_;
	}

protected:
	// Keeps the relationship between a parent node and its children.
	// std::map<TriRecord, std::vector<TriRecord> > dagNode_;

private:
	// For debugging,
	// check that the DAG is in consistent state.
	bool checkConsistent() const;

	/// Returns a set of triangles for the input point indexes.
	std::vector<TriRecord> findTrianglesWithEdge(int, int) const;

	/// Method to search the DAG for the triangle containing the point.
	/// This triangle will be subdivided into smaller triangles.
	// TriRecord findLeafNodeForPoint(int);

	// This was used in `mypanelopengl` for when we wanted to
	// render the Delaunay things.
	// std::vector<TriRecord> getLeafNodes();

	/// The directed graph's pointSet.
	PointSetArray pointSet_;

	std::vector<DAGNode*> dagNodes_;
	DAGNode* root_;
};

#endif
