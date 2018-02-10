#include "delaunay/directedgraph.h"

#include <assert.h>

#include <iostream>
#include <stack>

#include "delaunay/longint/li.h"

#include "delaunay/trianglegeometry.h"

#include "tracing.h"

#ifndef NDEBUG
#define DIRECTEDGRAPH_CHECK
#endif

using std::shared_ptr;
using std::stack;
using std::vector;
using std::cout;
using std::endl;

using geometry::isOverlapping;



namespace delaunay {

/// Check that the nodes contains a tri with all vertices i,j,k.
template<typename I>
bool containsTri(const vector<shared_ptr<DAGNode<I>>>& nodes, int i, int j, int k) {
	for (const shared_ptr<DAGNode<I>>& node : nodes) {
		TriRecord tri = node->tri_;

		if (tri.hasPointIndex(i) &&
		    tri.hasPointIndex(j) &&
		    tri.hasPointIndex(k)) {
			return true;
		}
	}
	return false;
}



template <typename I>
void outputTriList(const vector<shared_ptr<DAGNode<I>>>& nodes) {
	for (const shared_ptr<DAGNode<I>>& n : nodes) {
		int i,j,k;
		n->tri_.get(i,j,k);
		cout << n->tri_ << endl;
	}
}



// O(n^2),
// Use for an assert, to check we don't get both Tri(1,2,3) and Tri(3,2,1).
template <typename I>
bool trianglesUnique(const vector<shared_ptr<DAGNode<I>>>& nodes) {
	for (const shared_ptr<DAGNode<I>>& outerNode : nodes) {
		TriRecord outerTri = outerNode->tri_;

		for (const shared_ptr<DAGNode<I>>& innerNode : nodes) {
			TriRecord innerTri = innerNode->tri_;

			if (!(outerTri == innerTri) &&
				outerTri.sameVertices(innerTri)) {
				return false;
			}
		}
	}

	return true;
}



template <typename I>
bool leavesDoNotOverlap(const PointSetArray<I>& pointSet, const vector<shared_ptr<DAGNode<I>>>& nodes) {
	// Find the leaf nodes
	vector<shared_ptr<DAGNode<I>>> leafNodes;

	for (const shared_ptr<DAGNode<I>>& node : nodes) {
		if (node->isLeaf()) {
			leafNodes.push_back(node);
		}
	}

	// Check the leaf nodes never intersect with each other
	for (const shared_ptr<DAGNode<I>>& outerNode : leafNodes) {
		TriRecord outerTri = outerNode->tri_;

		for (const shared_ptr<DAGNode<I>>& innerNode : leafNodes) {
			TriRecord innerTri = innerNode->tri_;

			if (!(outerTri == innerTri) &&
			    isOverlapping(intersectsTriangle(pointSet, outerTri, innerTri))) {
				return false;
			}
		}
	}

	return true;
}



// Expensive,
// but check that the DirectedGraph is in good state.
template <typename I>
bool DirectedGraph<I>::checkConsistent() const {
	TRACE("[dag.checkConsistent()]");

	assert(trianglesUnique(dagNodes_));

	for (const shared_ptr<DAGNode<I>>& node : dagNodes_) {
		TriRecord tri = node->tri_;

		assert(isTriangleCCW(pointSet_, tri));
	}

	assert(leavesDoNotOverlap(pointSet_, dagNodes_));

	// Check, among the leaves, each edge occurs at most twice.
	for (const shared_ptr<DAGNode<I>>& node : dagNodes_) {
		TriRecord tri = node->tri_;

		int pIdx1, pIdx2, pIdx3;
		tri.get(pIdx1, pIdx2, pIdx3);

		if (node->isLeaf()) {
			// assert(findTrianglesWithEdge(pIdx1, pIdx2).size() <= 2);
			// assert(findTrianglesWithEdge(pIdx2, pIdx3).size() <= 2);
			// assert(findTrianglesWithEdge(pIdx1, pIdx3).size() <= 2);
		}
	}


	cout << "  [dag.checkConsistent()]" << " *** Directed Graph checked to be in consistent state ***" << endl; // DELAUNAY_TRACE_OUTPUT
	return true;
}



// It's slightly awkward, but we assume that the PointSet we're
// given is constant for rest of DAG;
template <typename I>
DirectedGraph<I>::DirectedGraph(const PointSetArray<I>& inputPointSet) {
	// TODO: If keep a separate copy of inputPointSet,
	// then wouldn't need delaunay.cpp to use `- 3` magic.

	// XXX Should use copy-c'tor for PointSetArray (test first?).
	// Copy points from the input set to Delaunay point set
	for (int i = 1; i <= inputPointSet.noPt(); i++) {
		I x, y;
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
	TriRecord boundingTri(boundingTriPt1,
	                      boundingTriPt1 + 1,
	                      boundingTriPt1 + 2);
	root_ = shared_ptr<DAGNode<I>>(new DAGNode<I>(boundingTri));
	root_->fIndex_ = trist_.addLinkedTri(boundingTri);
	dagNodes_.push_back(root_);
}



// This method returns the set of triangles the input point belongs to.
// Used by createVoronoi; I'd rather deprecate it.
template <typename I>
vector<TriRecord> DirectedGraph<I>::findTrianglesWithVertex(int pIdx1) const {
	vector<shared_ptr<DAGNode<I>>> nodes = DAGNode<I>::leafNodesContainingPoint(root_, pointSet_, pIdx1);

	vector<TriRecord> outputList;

	for (const shared_ptr<DAGNode<I>>& node : nodes) {
		outputList.push_back(node->tri_);
	}

	return outputList;
}



template <typename I>
int DirectedGraph<I>::findAdjacentTriangle(int pIdx1, int pIdx2, int pIdx3) const {
	TriRecord tri(pIdx1, pIdx2, pIdx3);
	vector<shared_ptr<DAGNode<I>>> triangles = DAGNode<I>::leafNodesContainingEdge(root_, pointSet_, pIdx2, pIdx3);

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
template <typename I>
void DirectedGraph<I>::legalizeEdge(int pIdx1, int pIdx2, int pIdx3) {
// TODO: legalizeEdge could be even quicker if we know adj. tri already
	TRACE("[dag.legalizeEdge(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ")]");
#ifdef DELAUNAY_CHECK

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



template <typename I>
void DirectedGraph<I>::addVertex(int pIdx) {
	TRACE(" [dag.addVertex(pointIndex=" << pIdx << ")]");
#ifdef DIRECTEDGRAPH_CHECK
#endif

	// Seek the lowest DAGNode which contains the point.
	vector<shared_ptr<DAGNode<I>>> leaves = DAGNode<I>::leafNodesContainingPoint(root_, pointSet_, pIdx);

#ifdef DIRECTEDGRAPH_CHECK
	cout << "DIRECTEDGRAPH_CHECK: [dag.addVertex(pointIndex=" << pIdx << ")] numLeafNodes containing pt: " << leaves.size() << endl;
	assert(leaves.size() == 1 || leaves.size() == 2);
#endif

	if (leaves.size() == 1) {
		// New point fits cleanly within another triangle,
		// split the triangle into three.

		shared_ptr<DAGNode<I>> node = leaves[0];  // IJK

		TriRecord parentTri = node->tri_;
		int parentIdx1, parentIdx2, parentIdx3;
		parentTri.get(parentIdx1, parentIdx2, parentIdx3);

		// Construct 3 TriRecords, one for each child triangle
		// ASSUMPTION that points for TriRecord are CCW
		shared_ptr<DAGNode<I>> child1(new DAGNode<I>(TriRecord(pIdx, parentIdx1, parentIdx2)));  // RIJ
		shared_ptr<DAGNode<I>> child2(new DAGNode<I>(TriRecord(pIdx, parentIdx2, parentIdx3)));  // RJK
		shared_ptr<DAGNode<I>> child3(new DAGNode<I>(TriRecord(pIdx, parentIdx3, parentIdx1)));  // RKI
		node->children_.push_back(child1);
		node->children_.push_back(child2);
		node->children_.push_back(child3);

		// Add to instance's list of dagNodes
		dagNodes_.push_back(child1);
		dagNodes_.push_back(child2);
		dagNodes_.push_back(child3);

		// Update triangulation
		addVertexInTri(trist_,
		               node->fIndex_,
		               child1,
		               child2,
		               child3);

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
		shared_ptr<DAGNode<I>> nodeIJK = leaves[0];
		shared_ptr<DAGNode<I>> nodeILJ = leaves[1];

		// need to sort out the point indices
		int iIdx, jIdx, kIdx, lIdx;
		getIndicesKIJL(nodeIJK->tri_, nodeILJ->tri_, kIdx, iIdx, jIdx, lIdx);

		// Create triangles RJK, RKI, RIL, RLJ
		shared_ptr<DAGNode<I>> nodeRJK(new DAGNode<I>(TriRecord(pIdx, jIdx, kIdx)));
		shared_ptr<DAGNode<I>> nodeRKI(new DAGNode<I>(TriRecord(pIdx, kIdx, iIdx)));
		shared_ptr<DAGNode<I>> nodeRIL(new DAGNode<I>(TriRecord(pIdx, iIdx, lIdx)));
		shared_ptr<DAGNode<I>> nodeRLJ(new DAGNode<I>(TriRecord(pIdx, lIdx, jIdx)));

		nodeIJK->children_.push_back(nodeRJK);
		nodeIJK->children_.push_back(nodeRKI);
		nodeILJ->children_.push_back(nodeRIL);
		nodeILJ->children_.push_back(nodeRLJ);

		dagNodes_.push_back(nodeRJK);
		dagNodes_.push_back(nodeRKI);
		dagNodes_.push_back(nodeRIL);
		dagNodes_.push_back(nodeRLJ);

		// Update triangulation
		addVertexOnEdge(trist_,
		                nodeIJK->fIndex_,
		                nodeILJ->fIndex_,
		                nodeRJK,
		                nodeRKI,
		                nodeRIL,
		                nodeRLJ);

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
template<typename I>
void DirectedGraph<I>::flipTriangles(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
	TRACE("[dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]");

	// XXX finish rename in DGraph::flipTri
	// shared edge ij; 124=kij is ccw, 423=jil is ccw.
	int kIdx = pIdx1;
	int iIdx = pIdx2;
	int lIdx = pIdx3;
	int jIdx = pIdx4;
#ifdef DIRECTEDGRAPH_CHECK

	cout << "DIRECTEDGRAPH_CHECK:  [dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]" << " Points: " << endl;
	cout << "DIRECTEDGRAPH_CHECK:  [dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]" << " 1. " << pointSet_[pIdx1] << endl;
	cout << "DIRECTEDGRAPH_CHECK:  [dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]" << " 2. " << pointSet_[pIdx2] << endl;
	cout << "DIRECTEDGRAPH_CHECK:  [dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]" << " 3. " << pointSet_[pIdx3] << endl;
	cout << "DIRECTEDGRAPH_CHECK:  [dag.flipTriangles(" << pIdx1 << ", " << pIdx2 << ", " << pIdx3 << ", " << pIdx4 << ")]" << " 4. " << pointSet_[pIdx4] << endl;

	assert(containsTri(dagNodes_, pIdx1, pIdx2, pIdx4));
	assert(isTriangleCCW(pointSet_, TriRecord(pIdx1, pIdx2, pIdx4)));
	assert(containsTri(dagNodes_, pIdx4, pIdx2, pIdx3));
	assert(isTriangleCCW(pointSet_, TriRecord(pIdx4, pIdx2, pIdx3)));
#endif

	// Seek the lowest DAGNode which contains the point.
	vector<shared_ptr<DAGNode<I>>> nodes = DAGNode<I>::leafNodesContainingEdge(root_, pointSet_, pIdx2, pIdx4);
#ifdef DIRECTEDGRAPH_CHECK
	// cout << "DAG.flipTriangles, numLeafNodes containing edge: " << nodes.size() << endl;
	// outputTriList(nodes);
	assert(nodes.size() == 2);
#endif

	// Ensure nodeIJK is the one with kIdx
	int nodesIJKIdx = (nodes[0]->tri_.hasPointIndex(kIdx)) ? 0 : 1;
	int nodesJILIdx = 1 - nodesIJKIdx;

	shared_ptr<DAGNode<I>> nodeIJK = nodes[nodesIJKIdx];
	shared_ptr<DAGNode<I>> nodeJIL = nodes[nodesJILIdx];

	// impl ASSUMPTION that TriR(x,y,z) == TriR(x,z,y), etc.
	// (as used in DirectedGraph).

	// swap 24 edge with 13 edge
	// ASSUMPTION that points for TriRecord are CCW
	// flip <abd>,<dbc> adds 2 children to each, <abc>,<acd> (preserves CCW)
	TriRecord triILK(iIdx, lIdx, kIdx);
	TriRecord triLJK(lIdx, jIdx, kIdx);
#ifdef DIRECTEDGRAPH_CHECK
	assert(isTriangleCCW(pointSet_, triILK));
	assert(isTriangleCCW(pointSet_, triLJK));
#endif

	shared_ptr<DAGNode<I>> nodeILK(new DAGNode<I>(triILK));
	shared_ptr<DAGNode<I>> nodeLJK(new DAGNode<I>(triLJK));

	nodeIJK->children_.push_back(nodeILK);
	nodeIJK->children_.push_back(nodeLJK);

	nodeJIL->children_.push_back(nodeILK);
	nodeJIL->children_.push_back(nodeLJK);

	// Add to instance's list of dagNodes
	dagNodes_.push_back(nodeILK);
	dagNodes_.push_back(nodeLJK);

	// Update triangulation
	delaunay::flipTriangles(trist_,
	                        nodeIJK->fIndex_,
	                        nodeJIL->fIndex_,
	                        nodeILK,
	                        nodeLJK);

#ifdef DIRECTEDGRAPH_CHECK
	checkConsistent();
#endif

	// legalizeEdge[FLIPTRIANGLE]

	legalizeEdge(pIdx1, pIdx2, pIdx3);
	legalizeEdge(pIdx1, pIdx3, pIdx4);
}



template<typename I>
vector<FIndex> DirectedGraph<I>::getLinkedTrianglesLookup() const {
	// All the existing tris from trist_
	vector<FIndex> tristTris = trist_.getLinkedTriangles();

	// PtIdx -> FIdx lookup
	vector<FIndex> fIndices(pointSet_.noPt());

	for (FIndex triIdx : tristTris) {
#ifdef TRIANGULATION_CHECK
		assert(trist_.isLinkedTri(triIdx));
#endif
		const LinkedTriangle& tri = trist_[triIdx];

		int pIdx1, pIdx2, pIdx3;
		tri.tri_.get(pIdx1, pIdx2, pIdx3);

		// Copy the linked tri
		fIndices[pIdx1 - 1] = triIdx;
		fIndices[pIdx2 - 1] = triIdx;
		fIndices[pIdx3 - 1] = triIdx;
	}

	return fIndices;
}



template<typename I>
void addVertexInTri(Triangulation& trist,
                    FIndex triIJK,
                    shared_ptr<DAGNode<I>> triRIJ,
                    shared_ptr<DAGNode<I>> triRJK,
                    shared_ptr<DAGNode<I>> triRKI) {
	TRACE("[addVertexInTri(trist, triIJK, *triRIJ, *triRJK, *triRKI)]");

	// Add the new triangles
	triRIJ->fIndex_ = trist.addLinkedTri(triRIJ->tri_);
	triRJK->fIndex_ = trist.addLinkedTri(triRJK->tri_);
	triRKI->fIndex_ = trist.addLinkedTri(triRKI->tri_);

	// It can be assumed that call to this fn,
	// the given shared_ptr<DAGNode> contain points in order.
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

#ifdef TRIANGULATION_CHECK
	assert(trist.checkConsistent());
#endif
}



template<typename I>
void addVertexOnEdge(Triangulation& trist,
                     FIndex triIJK,
                     FIndex triILJ,
                     shared_ptr<DAGNode<I>> triRJK,
                     shared_ptr<DAGNode<I>> triRKI,
                     shared_ptr<DAGNode<I>> triRIL,
                     shared_ptr<DAGNode<I>> triRLJ) {
	TRACE("[dag.addVertexOnEdge(trist, triIJK, triILJ, triRJK, triRKI, triRIL, triRLJ)]");

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

#ifdef TRIANGULATION_CHECK
	assert(trist.checkConsistent());
#endif
}



template<typename I>
void flipTriangles(Triangulation& trist,
                   FIndex triIJK,
                   FIndex triJIL,
                   shared_ptr<DAGNode<I>> triILK,
                   shared_ptr<DAGNode<I>> triLJK) {
	TRACE("[flipTriangles(trist, ijk, jil, *ilk, *ljk)]");

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

#ifdef TRIANGULATION_CHECK
	assert(trist.checkConsistent());
#endif
}



// Instantiate functions, methods
template bool containsTri<LongInt>(const vector<shared_ptr<DAGNode<LongInt>>>&, int, int, int);
template void outputTriList<LongInt>(const vector<shared_ptr<DAGNode<LongInt>>>&);
template bool trianglesUnique<LongInt>(const vector<shared_ptr<DAGNode<LongInt>>>& nodes);
template bool leavesDoNotOverlap<LongInt>(const PointSetArray<LongInt>&, const vector<shared_ptr<DAGNode<LongInt>>>&);

template void addVertexInTri<LongInt>(Triangulation&, FIndex, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>);
template void addVertexOnEdge<LongInt>(Triangulation&, FIndex, FIndex, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>);
template void flipTriangles<LongInt>(Triangulation&, FIndex, FIndex, shared_ptr<DAGNode<LongInt>>, shared_ptr<DAGNode<LongInt>>);

template class DirectedGraph<LongInt>;

template bool containsTri<double>(const vector<shared_ptr<DAGNode<double>>>&, int, int, int);
template void outputTriList<double>(const vector<shared_ptr<DAGNode<double>>>&);
template bool trianglesUnique<double>(const vector<shared_ptr<DAGNode<double>>>& nodes);
template bool leavesDoNotOverlap<double>(const PointSetArray<double>&, const vector<shared_ptr<DAGNode<double>>>&);

template void addVertexInTri<double>(Triangulation&, FIndex, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>);
template void addVertexOnEdge<double>(Triangulation&, FIndex, FIndex, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>);
template void flipTriangles<double>(Triangulation&, FIndex, FIndex, shared_ptr<DAGNode<double>>, shared_ptr<DAGNode<double>>);

template class DirectedGraph<double>;

}

