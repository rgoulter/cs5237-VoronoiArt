#include "directedgraph.h"

#include <assert.h>

#include <iostream>
#include <stack>

using std::stack;
using std::vector;
using std::cout;
using std::endl;



// O(log n) impl.
vector<DAGNode*> DAGNode::leafNodesContainingPoint(DAGNode* root, const PointSetArray& pointSet, int pIdx) {
	vector<DAGNode*> outputList;

	stack<DAGNode*> stk;
	stk.push(root);

	while (!stk.empty()) {
		DAGNode* node = stk.top();
		stk.pop();

		if (node->isLeaf()) {
			outputList.push_back(node);
		}

		for (vector<DAGNode*>::iterator iter = node->children_.begin();
		     iter != node->children_.end();
		     ++iter) {
			DAGNode *childNode = *iter;

			int p1Idx, p2Idx, p3Idx;
			childNode->tri_.get(p1Idx, p2Idx, p3Idx);

			// TODO convenient if PSet.inTri used TriRecord
			if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx) > 0) {
				stk.push(childNode);
			} else if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx) == 0) {
				cout << "DAG.addVertex, CASE: pIdx *on* tri." << endl;
				stk.push(childNode);
			}
		}
	}

	return outputList;
}



vector<DAGNode*> DAGNode::leafNodesContainingEdge(DAGNode* root, const PointSetArray& pointSet, int pIdx1, int pIdx2) {
	vector<DAGNode*> outputList;

	stack<DAGNode*> stk;
	stk.push(root);

	while (!stk.empty()) {
		DAGNode* node = stk.top();
		stk.pop();

		if (node->isLeaf()) {
			outputList.push_back(node);
		}

		for (vector<DAGNode*>::iterator iter = node->children_.begin();
		     iter != node->children_.end();
		     ++iter) {
			DAGNode *childNode = *iter;

			int p1Idx, p2Idx, p3Idx;
			childNode->tri_.get(p1Idx, p2Idx, p3Idx);

			// TODO convenient if PSet.inTri used TriRecord
			if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx1) >= 0 &&
				pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx2) >= 0) {
				stk.push(childNode);
			}
		}
	}

	return outputList;
}



int DAGNode::findAdjacentTriangle(DAGNode* root, const PointSetArray& pointSet, int pIdx1, int pIdx2, int pIdx3) {
	vector<DAGNode*> nodes = DAGNode::leafNodesContainingEdge(root, pointSet, pIdx2, pIdx3);

	for (vector<DAGNode*>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
		DAGNode* node = *iter;
		const TriRecord& tri = node->tri_;

		for (int j = 0; j < 3; j++) { /// "each point of tri"
			int pointIdx = tri.pointIndexOf(j); /// its pointIdx

			if (pointIdx != pIdx1 &&
				pointIdx != pIdx2 &&
				pointIdx != pIdx3) {
				/// if that triangle point isn't of `abc` in args..
				return pointIdx;
			}
		}
	}

	return 0;
}




/// Check that the nodes contains a tri with all vertices i,j,k.
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



void outputTriList(const vector<DAGNode*>& nodes) {
	for (vector<DAGNode*>::const_iterator iter = nodes.begin();
	     iter != nodes.end();
	     ++iter) {
		DAGNode *n = *iter;
		int i,j,k;
		n->tri_.get(i,j,k);
		cout << n->tri_ << endl;
	}
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



bool leavesDoNotOverlap(const PointSetArray& pointSet, const vector<DAGNode*>& nodes) {
	// Find the leaf nodes
	vector<DAGNode*> leafNodes;
	for (vector<DAGNode*>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
		DAGNode* node = *iter;

		if (node->isLeaf()) {
			leafNodes.push_back(node);
		}
	}

	// Check the leaf nodes never intersect with each other
	for (vector<DAGNode*>::const_iterator iter = leafNodes.begin(); iter != leafNodes.end(); ++iter) {
		TriRecord outerTri = (*iter)->tri_;

		for (vector<DAGNode*>::const_iterator innerIter = leafNodes.begin(); innerIter != leafNodes.end(); ++innerIter) {
			TriRecord innerTri = (*iter)->tri_;

			if (!(outerTri == innerTri) &&
			    intersectsTriangle(pointSet, outerTri, innerTri)) {
				return false;
			}
		}
	}

	return true;
}



// Expensive,
// but check that the DirectedGraph is in good state.
bool DirectedGraph::checkConsistent() const {
	assert(trianglesUnique(dagNodes_));

	for (vector<DAGNode*>::const_iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode* node = *iter;
		TriRecord tri = node->tri_;

		assert(isTriangleCCW(pointSet_, tri));
	}

	assert(leavesDoNotOverlap(pointSet_, dagNodes_));

	// Check, among the leaves, each edge occurs at most twice.
	for (vector<DAGNode*>::const_iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode* node = *iter;
		TriRecord tri = node->tri_;

		int pIdx1, pIdx2, pIdx3;
		tri.get(pIdx1, pIdx2, pIdx3);

		if (node->isLeaf()) {
			assert(findTrianglesWithEdge(pIdx1, pIdx2).size() <= 2);
			assert(findTrianglesWithEdge(pIdx2, pIdx3).size() <= 2);
			assert(findTrianglesWithEdge(pIdx1, pIdx3).size() <= 2);
		}
	}


	cout << "*** Directed Graph checked to be in consistent state ***" << endl;
	return true;
}



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
vector<TriRecord> DirectedGraph::findTrianglesWithVertex(int pIdx1) const {
	vector<TriRecord> outputlist;

	// O(n)
	for (vector<DAGNode*>::const_iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
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
vector<TriRecord> DirectedGraph::findTrianglesWithEdge(int pIdx1, int pIdx2) const {
	vector<TriRecord> outputlist;

	// O(n)
	for (vector<DAGNode*>::const_iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *node = *iter;
		TriRecord checkTriangle = node->tri_;

		if (node->isLeaf() &&
			checkTriangle.hasPointIndex(pIdx1)) {
			if (checkTriangle.hasPointIndex(pIdx2)) {
				outputlist.push_back(checkTriangle);
			}
		}
	}

	// cout << "dag.findTrisWEdge: " << pIdx1 << ", " << pIdx2 << endl;
	// cout << "dag.findTrisWEdge, output size = " << outputlist.size() << endl;
	assert(outputlist.size() == 2 || pIdx1 >= pointSet_.noPt() - 3 || pIdx2 >= pointSet_.noPt() - 3);

	return outputlist;
}



int DirectedGraph::findAdjacentTriangle(int pIdx1, int pIdx2, int pIdx3) const {
	vector<TriRecord> triangles = findTrianglesWithEdge(pIdx2, pIdx3);

	for (unsigned int i = 0; i < triangles.size(); i++) { /// "each triangle"
		for (int j = 0; j < 3; j++) { /// "each point of this triangle.."
			int pointIdx = triangles[i].pointIndexOf(j); /// its pointIdx

			if (pointIdx != pIdx1 && pointIdx != pIdx2 && pointIdx != pIdx3) {
				/// if that triangle point isn't of `abc` in args..
				return pointIdx;
			}
		}
	}

	return 0; // 0 is a 'bad index'; i.e. not found.
}



// This method adds children nodes to the specified parent node.
// This is only for new point additions and not for flipping.
// Use findLeafNodeforPoint to find parent,
// and then create child node for this triangle.
/*
 * From bouding tri (root), find the next tri which contains the point.
 */
TriRecord DirectedGraph::addVertex(int pIdx) {
	cout << "DAG.addVertex(" << pIdx << ")" << endl;

	// Seek the lowest DAGNode which contains the point.
	vector<DAGNode*> leaves = DAGNode::leafNodesContainingPoint(root_, pointSet_, pIdx);
	cout << "DAG.addVertex, numLeafNodes containing pt: " << leaves.size() << endl;

	DAGNode *node = leaves[0];  // Am assuming case of multiple is rare...


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
	dagNodes_.push_back(child2);
	dagNodes_.push_back(child3);
	checkConsistent();

	return parentTri;
}



// Please note that the edge to be flipped here is the 2nd and 4th parameters.
//
// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd>
//
// the shared edge bd gets replaced with shared edge ac
void DirectedGraph::flipTriangles(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
	cout << "DAG::flipTris, args=" << pIdx1 << "," << pIdx2 << "," << pIdx3 << "," << pIdx4 << "." << endl;

	cout << "Points: " << endl;
	cout << " 1. " << pointSet_[pIdx1] << endl;
	cout << " 2. " << pointSet_[pIdx2] << endl;
	cout << " 3. " << pointSet_[pIdx3] << endl;
	cout << " 4. " << pointSet_[pIdx4] << endl;

	assert(containsTri(dagNodes_, pIdx1, pIdx2, pIdx4));
	assert(isTriangleCCW(pointSet_, TriRecord(pIdx1, pIdx2, pIdx4)));
	assert(containsTri(dagNodes_, pIdx4, pIdx2, pIdx3));
	assert(isTriangleCCW(pointSet_, TriRecord(pIdx4, pIdx2, pIdx3)));

	// Seek the lowest DAGNode which contains the point.
	vector<DAGNode*> nodes = DAGNode::leafNodesContainingEdge(root_, pointSet_, pIdx2, pIdx4);
	// cout << "DAG.flipTriangles, numLeafNodes containing edge: " << nodes.size() << endl;
	// outputTriList(nodes);
	assert(nodes.size() == 2);


	DAGNode *abdNode = nodes[0]; // <124>
	DAGNode *dbcNode = nodes[1]; // <423>

	// impl ASSUMPTION that TriR(x,y,z) == TriR(x,z,y), etc.
	// (as used in DirectedGraph).

	// swap 24 edge with 13 edge
	// ASSUMPTION that points for TriRecord are CCW
	// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd> (preserves CCW)
	TriRecord abcTri(pIdx1, pIdx2, pIdx3);
	assert(isTriangleCCW(pointSet_, abcTri));
	TriRecord acdTri(pIdx1, pIdx3, pIdx4);
	assert(isTriangleCCW(pointSet_, acdTri));

	DAGNode *abcNode = new DAGNode(abcTri);
	DAGNode *acdNode = new DAGNode(acdTri);

	abdNode->children_.push_back(abcNode);
	abdNode->children_.push_back(acdNode);

	dbcNode->children_.push_back(abcNode);
	dbcNode->children_.push_back(acdNode);

	// Add to instance's list of dagNodes
	dagNodes_.push_back(abcNode);
	dagNodes_.push_back(acdNode);
	checkConsistent();
}

