#ifndef DELAUNAY_DIRECTEDGRAPHH
#define DELAUNAY_DIRECTEDGRAPHH

#include <iostream>
#include <memory>
#include <vector>

#include "delaunay/longint/li.h"

#include "delaunay/dagnode.h"
#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/triangulation.h"



namespace delaunay {

/* This Directed acyclic graph maintains a point location structure, which helps us find out the triangle in which
the current point under consideration is.

The leaf nodes of this graph gives the triangles in the current state of triangulation. The top node is the
bounding triangle of all the points in the delaunay triangulation.

Whenever a new point is added and triangulated, this graph should get children nodes from the parent triangle that
got deleted.
*/


class DirectedGraph {
public:
	DirectedGraph(const PointSetArray<LongInt>&);

	/// Finds the linked delaunay triangles for the input point index
	std::vector<TriRecord> findTrianglesWithVertex(int) const;

	/// Method to add new children triangles to a parent triangle.
	void addVertex(int);

	/// Create children nodes with 2 parents in case of edge flipping.
	void flipTriangles(int, int, int, int);

	const PointSetArray<LongInt>& getPointSet() const {
		return pointSet_;
	}

	/// Returns an array such-that each pointIdx refers to
	/// some LinkedTri which contains that pointIdx
	std::vector<FIndex> getLinkedTrianglesLookup() const;

	const Triangulation& getTriangulation() const { return trist_; }

protected:
	// Keeps the relationship between a parent node and its children.
	// std::map<TriRecord, std::vector<TriRecord> > dagNode_;

private:
	// For debugging,
	// check that the DAG is in consistent state.
	bool checkConsistent() const;

	/// Return a pointIdx for a triangle which shares pIdx2, pIdx3,
	/// but not pIdx1.
	/// Returns `0` if couldn't find an adjacent triangle.
	int findAdjacentTriangle(int pIdx1, int pIdx2, int pIdx3) const;

	void legalizeEdge(int, int, int);

	/// Returns a set of triangles for the input point indexes.
	// std::vector<TriRecord> findTrianglesWithEdge(int, int) const;

	/// Method to search the DAG for the triangle containing the point.
	/// This triangle will be subdivided into smaller triangles.
	// TriRecord findLeafNodeForPoint(int);

	// This was used in `mypanelopengl` for when we wanted to
	// render the Delaunay things.
	// std::vector<TriRecord> getLeafNodes();

	/// The directed graph's pointSet.
	PointSetArray<LongInt> pointSet_;
	Triangulation trist_;

	std::vector<std::shared_ptr<DAGNode<LongInt>>> dagNodes_;
	std::shared_ptr<DAGNode<LongInt>> root_;
};



// std::shared_ptr<DAGNode> is used so that we can update its FIndex

void addVertexInTri(Triangulation& trist,
                    FIndex triIJK,
                    std::shared_ptr<DAGNode<LongInt>> triRIJ,
                    std::shared_ptr<DAGNode<LongInt>> triRJK,
                    std::shared_ptr<DAGNode<LongInt>> triRKI);



void addVertexOnEdge(Triangulation& trist,
                     FIndex triIJK,
                     FIndex triILJ,
                     std::shared_ptr<DAGNode<LongInt>> triRJK,
                     std::shared_ptr<DAGNode<LongInt>> triRKI,
                     std::shared_ptr<DAGNode<LongInt>> triRIL,
                     std::shared_ptr<DAGNode<LongInt>> triRLJ);



void flipTriangles(Triangulation& trist,
                   FIndex triIJK,
                   FIndex triJIL,
                   std::shared_ptr<DAGNode<LongInt>> triILK,
                   std::shared_ptr<DAGNode<LongInt>> triLJK);

}

#endif
