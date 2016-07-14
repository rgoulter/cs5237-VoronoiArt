#include "delaunay/directedgraph.h"

#include <assert.h>

#include <iostream>
#include <stack>

#include "delaunay/trianglegeometry.h"

#ifndef NDEBUG
#define DIRECTEDGRAPH_CHECK
#endif

using std::stack;
using std::vector;
using std::cout;
using std::endl;



namespace delaunay {

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
			// assert(findTrianglesWithEdge(pIdx1, pIdx2).size() <= 2);
			// assert(findTrianglesWithEdge(pIdx2, pIdx3).size() <= 2);
			// assert(findTrianglesWithEdge(pIdx1, pIdx3).size() <= 2);
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
}



DirectedGraph::~DirectedGraph() {
	for (vector<DAGNode*>::iterator iter = dagNodes_.begin(); iter != dagNodes_.end(); ++iter) {
		DAGNode *ptr = *iter;
		delete ptr;
	}
}



// This method returns the set of triangles the input point belongs to.
// Used by createVoronoi; I'd rather deprecate it.
vector<TriRecord> DirectedGraph::findTrianglesWithVertex(int pIdx1) const {
	vector<DAGNode*> nodes = DAGNode::leafNodesContainingPoint(root_, pointSet_, pIdx1);

	vector<TriRecord> outputList;

	for (vector<DAGNode*>::iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
		outputList.push_back((*iter)->tri_);
	}

	return outputList;
}



int DirectedGraph::findAdjacentTriangle(int pIdx1, int pIdx2, int pIdx3) const {
	TriRecord tri(pIdx1, pIdx2, pIdx3);
	vector<DAGNode*> triangles = DAGNode::leafNodesContainingEdge(root_, pointSet_, pIdx2, pIdx3);

	for (unsigned int i = 0; i < triangles.size(); i++) { /// "each triangle"
		int pIdx = tri.vertexNotSharedWith(triangles[i]->tri_);

		if (pIdx > 0) {
			return pIdx;
		}
	}

	return 0; // 0 is a 'bad index'; i.e. not found.
}



/// Do ??? with dag, and points a,b,c
/// What use does pIdx1 serve? Freshly inserted point?
/// => make sure edge bc is Locally Delaunay,
///    ??? and that the flips we make keep things Locally Delaunay
void DirectedGraph::legalizeEdge(int pIdx1, int pIdx2, int pIdx3) {
// TODO: legalizeEdge could be even quicker if we know adj. tri already
#ifdef DELAUNAY_CHECK
	cout << "DTri::legalizeEdge, " << pIdx1 << ", " << pIdx2 << "," << pIdx3 << endl;

	assert(isTriangleCCW(pointSet_, TriRecord(pIdx1, pIdx2, pIdx3)));
#endif

	int p4 = findAdjacentTriangle(pIdx1, pIdx2, pIdx3);

	if (p4 > 0) {
		// Presumably delaunayPointSet === dag.getPointSet()
		// so this is legit

		/// if this point is in the circumcircle of abc triangle..
		if (pointSet_.inCircle(pIdx1, pIdx2, pIdx3, p4) > 0) {
			///> want to replace ij w/ kr
			// abd, dbc must be triangles.
			// TRI = pidx1,2,3;
			// TRI = pidx2,3,4
			// dag.flipTriangles(pIdx1, pIdx2, pIdx3, p4);
			flipTriangles(pIdx1, pIdx2, p4, pIdx3);
		}
	}
}



void DirectedGraph::addVertex(int pIdx) {
#ifdef DIRECTEDGRAPH_CHECK
	cout << "DAG.addVertex(" << pIdx << ")" << endl;
#endif

	// Seek the lowest DAGNode which contains the point.
	vector<DAGNode*> leaves = DAGNode::leafNodesContainingPoint(root_, pointSet_, pIdx);

#ifdef DIRECTEDGRAPH_CHECK
	cout << "DAG.addVertex, numLeafNodes containing pt: " << leaves.size() << endl;
	assert(leaves.size() == 1 || leaves.size() == 2);
#endif

	if (leaves.size() == 1) {
		// New point fits cleanly within another triangle,
		// split the triangle into three.

		DAGNode *node = leaves[0];

		TriRecord parentTri = node->tri_;
		int parentIdx1, parentIdx2, parentIdx3;
		parentTri.get(parentIdx1, parentIdx2, parentIdx3);

		// Construct 3 TriRecords, one for each child triangle
		// ASSUMPTION that points for TriRecord are CCW
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

#ifdef DIRECTEDGRAPH_CHECK
		checkConsistent();
#endif

		// legalizeEdge[ADDVERT(A)]

		/// edges 12, 13, 23 are the "link" of the inserted point.
		/// So, here we 'flip edges' until things are locally delaunday.
		legalizeEdge(pIdx, parentIdx1, parentIdx2);
		legalizeEdge(pIdx, parentIdx2, parentIdx3);
		legalizeEdge(pIdx, parentIdx3, parentIdx1);
	} else if (leaves.size() == 2) {
		// New point on the edge of other triangles,
		// split the two triangles to get four triangles total.

		// new point R lies on edge IJ
		DAGNode* nodeIJK = leaves[0];
		DAGNode* nodeILJ = leaves[1];

		// need to sort out the point indices
		int iIdx, jIdx, kIdx, lIdx;
		getIndicesKIJL(nodeIJK->tri_, nodeILJ->tri_, kIdx, iIdx, jIdx, lIdx);

		// Create triangles RJK, RKI, RIL, RLJ
		DAGNode *nodeRJK = new DAGNode(TriRecord(pIdx, jIdx, kIdx));
		DAGNode *nodeRKI = new DAGNode(TriRecord(pIdx, kIdx, iIdx));
		DAGNode *nodeRIL = new DAGNode(TriRecord(pIdx, iIdx, lIdx));
		DAGNode *nodeRLJ = new DAGNode(TriRecord(pIdx, lIdx, jIdx));

		nodeIJK->children_.push_back(nodeRJK);
		nodeIJK->children_.push_back(nodeRKI);
		nodeILJ->children_.push_back(nodeRIL);
		nodeILJ->children_.push_back(nodeRLJ);

		dagNodes_.push_back(nodeRJK);
		dagNodes_.push_back(nodeRKI);
		dagNodes_.push_back(nodeRIL);
		dagNodes_.push_back(nodeRLJ);

#ifdef DIRECTEDGRAPH_CHECK
		checkConsistent();
#endif

		// legalizeEdge[ADDVERT(B)]
		// legalize il, lj, jk, ki
		legalizeEdge(pIdx, jIdx, kIdx);
		legalizeEdge(pIdx, kIdx, iIdx);
		legalizeEdge(pIdx, iIdx, lIdx);
		legalizeEdge(pIdx, lIdx, jIdx);
	}
}



// Please note that the edge to be flipped here is the 2nd and 4th parameters.
//
// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd>
//
// the shared edge bd gets replaced with shared edge ac
void DirectedGraph::flipTriangles(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
#ifdef DIRECTEDGRAPH_CHECK
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
#endif

	// Seek the lowest DAGNode which contains the point.
	vector<DAGNode*> nodes = DAGNode::leafNodesContainingEdge(root_, pointSet_, pIdx2, pIdx4);
#ifdef DIRECTEDGRAPH_CHECK
	// cout << "DAG.flipTriangles, numLeafNodes containing edge: " << nodes.size() << endl;
	// outputTriList(nodes);
	assert(nodes.size() == 2);
#endif


	DAGNode *abdNode = nodes[0]; // <124>
	DAGNode *dbcNode = nodes[1]; // <423>

	// impl ASSUMPTION that TriR(x,y,z) == TriR(x,z,y), etc.
	// (as used in DirectedGraph).

	// swap 24 edge with 13 edge
	// ASSUMPTION that points for TriRecord are CCW
	// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd> (preserves CCW)
	TriRecord abcTri(pIdx1, pIdx2, pIdx3);
	TriRecord acdTri(pIdx1, pIdx3, pIdx4);
#ifdef DIRECTEDGRAPH_CHECK
	assert(isTriangleCCW(pointSet_, abcTri));
	assert(isTriangleCCW(pointSet_, acdTri));
#endif

	DAGNode *abcNode = new DAGNode(abcTri);
	DAGNode *acdNode = new DAGNode(acdTri);

	abdNode->children_.push_back(abcNode);
	abdNode->children_.push_back(acdNode);

	dbcNode->children_.push_back(abcNode);
	dbcNode->children_.push_back(acdNode);

	// Add to instance's list of dagNodes
	dagNodes_.push_back(abcNode);
	dagNodes_.push_back(acdNode);

#ifdef DIRECTEDGRAPH_CHECK
	checkConsistent();
#endif

	// legalizeEdge[FLIPTRIANGLE]

	legalizeEdge(pIdx1, pIdx2, pIdx3);
	legalizeEdge(pIdx1, pIdx3, pIdx4);
}



void addVertexInTri(Triangulation& trist,
                    FIndex triIJK,
                    DAGNode* triRIJ,
                    DAGNode* triRJK,
                    DAGNode* triRKI) {
	// Add the new triangles
	triRIJ->fIndex_ = trist.addLinkedTri(triRIJ->tri_);
	triRJK->fIndex_ = trist.addLinkedTri(triRJK->tri_);
	triRKI->fIndex_ = trist.addLinkedTri(triRKI->tri_);

	// It can be assumed that call to this fn,
	// the given DAGNode* contain points in order.
	//
	// But, no knowledge about the triangle referenced
	// by triIJK
	int rIdx, iIdx, jIdx;
	triRIJ->tri_.get(rIdx, iIdx, jIdx);

	const LinkedTriangle& ltriIJK = trist[triIJK];
	int edgeIdxIJ, edgeIdxJK, edgeIdxKI;
	ltriIJK.getEdgeIndices(iIdx, edgeIdxIJ, edgeIdxJK, edgeIdxKI);

	// Get reference to adjacent triangles,
	// (might not actually be triangles)
	FIndex adjIndexIJ = ltriIJK.links_[edgeIdxIJ];
	FIndex adjIndexJK = ltriIJK.links_[edgeIdxJK];
	FIndex adjIndexKI = ltriIJK.links_[edgeIdxKI];

	// unlink ... not necessary.

	// internal links (3x: RI, RJ, RK)
	trist.setLink(triRIJ->fIndex_, 2, triRJK->fIndex_); // RJ
	trist.setLink(triRJK->fIndex_, 2, triRKI->fIndex_); // RK
	trist.setLink(triRKI->fIndex_, 2, triRIJ->fIndex_); // RI

	// external links (3x: IJ, JK, KI)
	// PERF: if store what the other edge idx is, could save lookup.
	trist.setLink(triRIJ->fIndex_, 1, adjIndexIJ); // RJ
	trist.setLink(triRJK->fIndex_, 1, adjIndexJK); // RK
	trist.setLink(triRKI->fIndex_, 1, adjIndexKI); // RI

	// remove old triangles
	trist.removeLinkedTri(triIJK);
}



void addVertexOnEdge(Triangulation& trist,
                     FIndex triIJK,
                     FIndex triILJ,
                     DAGNode* triRJK,
                     DAGNode* triRKI,
                     DAGNode* triRIL,
                     DAGNode* triRLJ) {
	// Add the new triangles
	triRJK->fIndex_ = trist.addLinkedTri(triRJK->tri_);
	triRKI->fIndex_ = trist.addLinkedTri(triRKI->tri_);
	triRIL->fIndex_ = trist.addLinkedTri(triRIL->tri_);
	triRLJ->fIndex_ = trist.addLinkedTri(triRLJ->tri_);

	// Get the edge indicies (at least iIdx)
	int rIdx, iIdx, lIdx;
	triRIL->tri_.get(rIdx, iIdx, lIdx);

	const LinkedTriangle& ltriIJK = trist[triIJK];
	int ijkEdgeIdxIJ, ijkEdgeIdxJK, ijkEdgeIdxKI;
	ltriIJK.getEdgeIndices(iIdx, ijkEdgeIdxIJ, ijkEdgeIdxJK, ijkEdgeIdxKI);

	const LinkedTriangle& ltriILJ = trist[triILJ];
	int iljEdgeIdxIL, iljEdgeIdxLJ, iljEdgeIdxJI;
	ltriILJ.getEdgeIndices(iIdx, iljEdgeIdxIL, iljEdgeIdxLJ, iljEdgeIdxJI);

	// Get reference to adjacent triangles
	FIndex adjIndexKI = ltriIJK.links_[ijkEdgeIdxKI];
	FIndex adjIndexIL = ltriILJ.links_[iljEdgeIdxIL];
	FIndex adjIndexLJ = ltriILJ.links_[iljEdgeIdxLJ];
	FIndex adjIndexJK = ltriIJK.links_[ijkEdgeIdxJK];

	// internal links (4x: RK, RI, RL, RJ)
	trist.setLink(triRJK->fIndex_, 2, triRKI->fIndex_);  // RK
	trist.setLink(triRKI->fIndex_, 2, triRIL->fIndex_);  // RI
	trist.setLink(triRIL->fIndex_, 2, triRLJ->fIndex_);  // RL
	trist.setLink(triRLJ->fIndex_, 2, triRJK->fIndex_);  // RL

	// external links (4x: JK, KI, IL, LJ)
	// PERF: if store what the other edge idx is, could save lookup.
	trist.setLink(triRJK->fIndex_, 1, adjIndexJK);
	trist.setLink(triRKI->fIndex_, 1, adjIndexKI);
	trist.setLink(triRIL->fIndex_, 1, adjIndexIL);
	trist.setLink(triRLJ->fIndex_, 1, adjIndexLJ);

	// remove old triangles
	trist.removeLinkedTri(triIJK);
	trist.removeLinkedTri(triILJ);
}



void flipTriangles(Triangulation& trist,
                   FIndex triIJK,
                   FIndex triJIL,
                   DAGNode* triILK,
                   DAGNode* triLJK) {
	// Add the new triangles
	triILK->fIndex_ = trist.addLinkedTri(triILK->tri_);
	triLJK->fIndex_ = trist.addLinkedTri(triLJK->tri_);

	// Get the edge indicies (at least iIdx)
	int iIdx, lIdx, kIdx;
	triILK->tri_.get(iIdx, lIdx, kIdx);

	const LinkedTriangle& ltriIJK = trist[triIJK];
	int ijkEdgeIdxIJ, ijkEdgeIdxJK, ijkEdgeIdxKI;
	ltriIJK.getEdgeIndices(iIdx, ijkEdgeIdxIJ, ijkEdgeIdxJK, ijkEdgeIdxKI);

	const LinkedTriangle& ltriJIL = trist[triJIL];
	int jilEdgeIdxJI, jilEdgeIdxIL, jilEdgeIdxLJ;
	ltriJIL.getEdgeIndices(iIdx, jilEdgeIdxIL, jilEdgeIdxLJ, jilEdgeIdxJI);

	// Get reference to adjacent triangles
	FIndex adjIndexKI = ltriIJK.links_[ijkEdgeIdxKI];
	FIndex adjIndexIL = ltriJIL.links_[jilEdgeIdxIL];
	FIndex adjIndexLJ = ltriJIL.links_[jilEdgeIdxLJ];
	FIndex adjIndexJK = ltriIJK.links_[ijkEdgeIdxJK];

	// internal links (1x: KL)
	trist.setLink(triLJK->fIndex_, 2, triILK->fIndex_);

	// external links (4x: JK, KI, IL, LJ)
	// PERF: if store what the other edge idx is, could save lookup.
	trist.setLink(triLJK->fIndex_, 0, adjIndexLJ);  // LJ
	trist.setLink(triLJK->fIndex_, 1, adjIndexJK);  // JK
	trist.setLink(triILK->fIndex_, 0, adjIndexIL);  // IL
	trist.setLink(triILK->fIndex_, 2, adjIndexKI);  // KI

	// remove old triangles
	trist.removeLinkedTri(triIJK);
	trist.removeLinkedTri(triJIL);
}

}

