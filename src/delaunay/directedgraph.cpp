#include "directedgraph.h"

#include <assert.h>

#include <iostream>

using std::map;
using std::vector;



class DAGNode {
public:
	DAGNode(const TriRecord& tri) : tri_(tri) {}

	bool isLeaf() const { return children_.empty(); }

	TriRecord tri_;
	vector<DAGNode *> children_;
};



// It's slightly awkward, but we assume that the PointSet we're
// given is constant for rest of DAG;
DirectedGraph::DirectedGraph(const PointSetArray& inputPointSet) {
	// TODO: If keep a separate copy of inputPointSet,
	// then wouldn't need delaunay.cpp to use `- 3` magic.

	// XXX Should use copy-c'tor for PointSetArray (test first?).
	// Copy points from the input set to Delaunay point set
	for (int i = 1; i <= inputPointSet.noPt(); i++) {
		LongInt x, y;
		inputPointSet.getPoint(i, x, y);
		pointSet_.addPoint(x, y);
	}

	int numPoints = pointSet_.noPt();

	// Add a triangle which bounds all the points.
	findBoundingTri(pointSet_);

	// Construct root DAGNode as the bounding triangle.
	// (Delaunay Iteration algorithm can 'add' the 'actual' points,
	//  which is also slightly awkward, but whatever).

	// Points of bounding tri are the last three,
	// so, numPoints+1, numPoints+2, numPoints+3
	int boundingTriPt1 = numPoints + 1;
	root_ = new DAGNode(TriRecord(boundingTriPt1,
	                              boundingTriPt1 + 1,
	                              boundingTriPt1 + 2));
	dagNodes_.push_back(root_);

	// TODO I've no clue what this does / why it's here.
	// addVertex(delaunayPointSet.noPt());
}



DirectedGraph::~DirectedGraph() {
	// Go through 
	for (vector<DAGNode*>::iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *ptr = *iter;
		delete ptr;
	}

	// root_ never added to dagNodes_
	// since root_ is the bounding triangle,
	// and the bounding triangle vertices don't count.
	// delete root_;
}



// This method returns the set of triangles the input point belongs to.
vector<TriRecord> DirectedGraph::findTrianglesWithVertex(int pIdx1) {
	vector<TriRecord> outputlist;

	for (vector<DAGNode*>::iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *node = *iter;
		TriRecord checkTriangle = node->tri_;

		if (node->isLeaf() &&
			checkTriangle.hasPointIndex(pIdx1)) {
			outputlist.push_back(checkTriangle);
		}
	}

	return outputlist;
}



// This method returns the set of 2 triangles the input edge belongs to.
/*
 * w/ list of all DagNodes, (ergo, all triangles),
 * can filter through to return TriRecord
 */
vector<TriRecord> DirectedGraph::findTrianglesWithEdge(int pIdx1, int pIdx2) {
	vector<TriRecord> outputlist;

	for (vector<DAGNode*>::iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *node = *iter;
		TriRecord checkTriangle = node->tri_;

		if (node->isLeaf() &&
			checkTriangle.hasPointIndex(pIdx1)) {
			if (checkTriangle.hasPointIndex(pIdx2)) {
				outputlist.push_back(checkTriangle);
			}
		}
	}

	return outputlist;
}



// O(n^2),
// Use for an assert, to check we don't get both Tri(1,2,3) and Tri(3,2,1).
bool trianglesUnique(const vector<DAGNode*>& nodes) {
	typedef vector<DAGNode*>::const_iterator const_iter;
	for (const_iter iter = nodes.begin(); iter != nodes.end(); ++iter) {
		DAGNode* outerNode = *iter;
		TriRecord outerTri = outerNode->tri_;

		for (const_iter innerIter = nodes.begin(); innerIter != nodes.end(); ++innerIter) {
			DAGNode* innerNode = *innerIter;
			TriRecord innerTri = innerNode->tri_;

			if (!(outerTri == innerTri) &&
				outerTri.sameVertices(innerTri)) {
				return false;
			}
		}

	}

	return true;
}



// This method adds children nodes to the specified parent node.
// This is only for new point additions and not for flipping.
// Use findLeafNodeforPoint to find parent,
// and then create child node for this triangle.
/*
 * From bouding tri (root), find the next tri which contains the point.
 */
TriRecord DirectedGraph::addVertex(int pIdx) {
	std::cout << "addVertex 1, pIdx=" << pIdx << std::endl;

	// Seek the lowest DAGNode which contains the point.
	DAGNode *node = root_;

	while (!node->isLeaf()) {
		for (vector<DAGNode*>::iterator iter = node->children_.begin();
		     iter != node->children_.end();
		     ++iter) {
			DAGNode *childNode = *iter;

			int p1Idx, p2Idx, p3Idx;
			childNode->tri_.get(p1Idx, p2Idx, p3Idx);

			// TODO convenient if PSet.inTri used TriRecord
			if (pointSet_.inTri(p1Idx, p2Idx, p3Idx, pIdx)) {
				node = childNode;
				break; // break from iter loop
			}
		}
	}

	TriRecord parentTri = node->tri_;
	int parentIdx1, parentIdx2, parentIdx3;
	parentTri.get(parentIdx1, parentIdx2, parentIdx3);

	// Construct 3 TriRecords, one for each child triangle
	// XXX ASSUMPTION that points for TriRecord are CCW
	DAGNode *child1 = new DAGNode(TriRecord(parentIdx1, parentIdx2, pIdx)); // CCW
	DAGNode *child2 = new DAGNode(TriRecord(parentIdx2, parentIdx3, pIdx));
	DAGNode *child3 = new DAGNode(TriRecord(parentIdx3, parentIdx1, pIdx));
	node->children_.push_back(child1);
	node->children_.push_back(child2);
	node->children_.push_back(child3);

	// Add to instance's list of dagNodes
	dagNodes_.push_back(child1);
	assert(trianglesUnique(dagNodes_));
	dagNodes_.push_back(child2);
	assert(trianglesUnique(dagNodes_));
	dagNodes_.push_back(child3);
	assert(trianglesUnique(dagNodes_));

	return parentTri;
}



bool containsTri(const vector<DAGNode*>& nodes, int i, int j, int k) {
	for (vector<DAGNode*>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
		DAGNode *node = *iter;
		TriRecord tri = node->tri_;

		if (tri.hasPointIndex(i) &&
		    tri.hasPointIndex(j) &&
		    tri.hasPointIndex(k)) {
			return true;
		}
	}
	return false;
}



// Please note that the edge to be flipped here is the 2nd and 3rd parameters.
//
// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd>
//
// the shared edge bd gets replaced with shared edge ac
void DirectedGraph::flipTriangles(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
	std::cout << "DAG::flipTris, args=" << pIdx1 << "," << pIdx2 << "," << pIdx3 << "," << pIdx4 << "." << std::endl;

	assert(containsTri(dagNodes_, pIdx1, pIdx2, pIdx4));
	assert(containsTri(dagNodes_, pIdx4, pIdx2, pIdx3));

	assert(pIdx1 >= 1 && pIdx1 < 1000000); // XXX magic bound
	assert(pIdx2 >= 1 && pIdx2 < 1000000);
	assert(pIdx3 >= 1 && pIdx3 < 1000000);
	assert(pIdx4 >= 1 && pIdx4 < 1000000);

	// PERF: O(n) to find these two. Should probably be O(log n)
	// Copy & paste from findTrisWithEdge, since we want the DAGNode*.
	vector<DAGNode*> nodes;

	for (vector<DAGNode*>::iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *node = *iter;
		TriRecord checkTriangle = node->tri_;

		if (node->isLeaf() &&
			checkTriangle.hasPointIndex(pIdx2)) { // b
			if (checkTriangle.hasPointIndex(pIdx4)) { // d
				nodes.push_back(node);
			}
		}
	}

	// one edge shared by two triangles
	// => this *won't* be true, due to flipped nodes, duh.
	std::cout << "DAG::flipTris, before assert, size=" << (nodes.size()) << std::endl;
	for (vector<DAGNode*>::iterator iter = nodes.begin();
	     iter != nodes.end();
	     ++iter) {
		DAGNode *n = *iter;
		int i,j,k;
		n->tri_.get(i,j,k);
		std::cout << "Tri(" << i << "," << j << "," << k << ")" << std::endl;
	}
	assert(nodes.size() == 2);

	DAGNode *abdNode = nodes[0]; // <124>
	DAGNode *dbcNode = nodes[1]; // <423>

	// impl ASSUMPTION that TriR(x,y,z) == TriR(x,z,y), etc.
	// (as used in DirectedGraph).

	// swap 24 edge with 13 edge
	// ASSUMPTION that points for TriRecord are CCW
	// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd> (preserves CCW)
	DAGNode *abcNode = new DAGNode(TriRecord(pIdx1, pIdx2, pIdx3));
	DAGNode *acdNode = new DAGNode(TriRecord(pIdx1, pIdx3, pIdx4));

	abdNode->children_.push_back(abcNode);
	abdNode->children_.push_back(acdNode);

	dbcNode->children_.push_back(abcNode);
	dbcNode->children_.push_back(acdNode);

	// Add to instance's list of dagNodes
	dagNodes_.push_back(abcNode);
	assert(trianglesUnique(dagNodes_));
	dagNodes_.push_back(acdNode);
	assert(trianglesUnique(dagNodes_));
}

