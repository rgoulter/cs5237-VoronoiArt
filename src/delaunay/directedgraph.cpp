#include "directedgraph.h"

#include <assert.h>

#include <iostream>

using std::map;
using std::vector;



DirectedGraph::DirectedGraph(PointSetArray pSet) {
	triVertices_ = pSet;
}



// This method returns the set of triangles the input point belongs to.
// XXX I'm *fairly* sure that this fails in cases where given pIdx doesn't
//     belong to a leaf node.
//     WTF, implementation of *this* should be *trivial*.
vector<TriRecord> DirectedGraph::findLinkedNodes(int pIdx1) {
	vector<TriRecord> templist, outputlist;

	std::cout << "DAG::findLinkedNodes 1, " << pIdx1 << ", leafNodeList.size=" << (leafNodeList_.size()) << std::endl;

	// XXX FUCKING EVEN THIS SHIT DOESN'T WORK WHAT THE FUCK
	for (vector<TriRecord>::iterator iter = leafNodeList_.begin(); iter != leafNodeList_.end(); ++iter) {
		TriRecord checkTriangle = *iter;

		// trirecord has pIdx1?
		if (checkTriangle.hasPointIndex(pIdx1)) {
			templist.push_back(checkTriangle);
		}
	}

	std::cout << "DAG::findLinkedNodes 2" << std::endl;

	assert(templist.size() > 0); // XXX FAILS OMG
	TriRecord pickedTri = templist.front();
	TriRecord prevTri = templist.front();

	outputlist.push_back(pickedTri);
	int commonvert;
	int nextvert;

	std::cout << "DAG::findLinkedNodes 3" << std::endl;

	// get common vert? -> (0,1,2), or -1
	if (pIdx1 == pickedTri.pointIndexOf(0)) {
		commonvert = 0;
		nextvert = 1;
	} else if (pIdx1 == pickedTri.pointIndexOf(1)) {
		commonvert = 1;
		nextvert = 2;
	} else if (pIdx1 == pickedTri.pointIndexOf(2)) {
		commonvert = 2;
		nextvert = 0;
	}

	std::cout << "DAG::findLinkedNodes 3" << std::endl;

	// XXX MIGHTN'T EVEN FUCKING TERMINATE
	while (true) {
		std::cout << "DAG::findLinkedNodes 4.1" << std::endl;
		vector<TriRecord> fnextlist = findNodesForEdge(prevTri.pointIndexOf(commonvert), prevTri.pointIndexOf(nextvert));

		TriRecord nextTri =
			(fnextlist.front() == prevTri) ?
			fnextlist.back() :
			fnextlist.front();

		std::cout << "DAG::findLinkedNodes 4.2" << std::endl;

		// If the newly found triangle is the same as
		//    the very first triangle picked,
		// then a full cycle of sort has been completed.
		if (nextTri == pickedTri)
			break;

		std::cout << "DAG::findLinkedNodes 4.3" << std::endl;

		outputlist.push_back(nextTri);

		// Find an edge in nextTri that has the common vertex
		// but not shared with prevTri
		prevTri.nextEdge(commonvert, nextvert, nextTri);

		prevTri = nextTri;
	}

	return outputlist;
}



// This method returns the set of 2 triangles the input edge belongs to.
vector<TriRecord> DirectedGraph::findNodesForEdge(int pIdx1, int pIdx2) {
	vector<TriRecord> outputlist;


	for (vector<TriRecord>::iterator iter = leafNodeList_.begin(); iter != leafNodeList_.end(); ++iter) {
		TriRecord checkTriangle = *iter;

		int pIndex1, pIndex2, pIndex3;
		checkTriangle.get(pIndex1, pIndex2, pIndex3);

		if (checkTriangle.hasPointIndex(pIdx1)) {
			if (checkTriangle.hasPointIndex(pIdx2)) {
				outputlist.push_back(checkTriangle);
			}
		}
	}

	return outputlist;
}



// This method returns the triangle which encloses the input point.
// Used by `delaunay.delaunayIterationStep()`,
//         `DirectedGraph::addChildrenNodes`
TriRecord DirectedGraph::findLeafNodeForPoint(int pIdx) {
	assert(pIdx >= 1 && pIdx < 1000000);

	std::cout << "findLeafNodeForPt 1, " << pIdx << std::endl;

	vector<TriRecord> worklist;


	// If the root node has no edges, it is the bounding triangle.
	if (!orderedKeyList_.empty()) {
		TriRecord rootNode = orderedKeyList_.front();

		// Happens when the ordered list only has bounding triangle
		if (dagNode_.size() == 0) {
			worklist.push_back(rootNode);
		} else {
			worklist = dagNode_.find(rootNode) ->second;
		}
	} else {
		// Return the trirecord composed of last 3 entries from myPoints pointset. The incoming point ID is the last vertex of
		// the bounding triangle.
		TriRecord boundingTri(pIdx, pIdx - 1, pIdx - 2);

		return boundingTri;
	}

	// Iterate through the worklist and find the containing triangle. Once found, replace the worklist with
	// the new worklist of this triangle and continue iteration.

	std::cout << "findLeafNodeForPt 2" << std::endl;

	for (vector<TriRecord>::iterator iter = worklist.begin(); iter != worklist.end(); ) {
		std::cout << "findLeafNodeForPt 2.1" << std::endl;
		TriRecord checkTriangle = *iter;
		std::cout << "findLeafNodeForPt 2.2" << std::endl;

		// TODO inTri would benefit from using TriRecord
		int pIndex1, pIndex2, pIndex3;
		checkTriangle.get(pIndex1, pIndex2, pIndex3);

		int ret = triVertices_.inTri(pIndex1, pIndex2, pIndex3, pIdx);
		std::cout << "findLeafNodeForPt 2.3" << std::endl;

		// If ret is >=0, the point is inside the triangle
		if (ret >= 0) {
			std::cout << "findLeafNodeForPt 2.3A" << std::endl;

			return checkTriangle;
			// XXX WTF is the rest of the code for?
			// if (dagNode_.find(checkTriangle) != dagNode_.end()) {
			// 	// There could be error here.
			// 	// Does C++ allow us to change the object we are iterating through?
			// 	worklist = dagNode_.find(checkTriangle)->second;
			// 	iter = worklist.begin();
			// } else {
			// 	std::cout << "findLeafNodeForPt 2.3B" << std::endl;
			// 	return checkTriangle;
			// }
		} else {
			++iter;
		}

		std::cout << "findLeafNodeForPt 3" << std::endl;
	}

	std::cout << "findLeafNodeForPt 4, we never return anything!!" << std::endl;
	assert(false);
}



vector<TriRecord> DirectedGraph::getLeafNodes() {
	return leafNodeList_;
}



// This method adds children nodes to the specified parent node. This is only for new point additions and not for flipping.
void DirectedGraph::addChildrenNodes(int pIdx) {
	std::cout << "addChildrenNodes 1, pIdx=" << pIdx << std::endl;


	// Find triangle to which the point belongs. This function returns the bounding triangle in the case that the
	// point passed here is the end vertex of the bounding triangle
	TriRecord containingTriangle = findLeafNodeForPoint(pIdx);

	std::cout << "addChildrenNodes 2" << std::endl;


	// Declare iterator for the dagNode, and the vector of TriRecords for the children of the node.
	// map< TriRecord, vector<TriRecord> >::iterator iter;
	vector<TriRecord> existingChildren;

	// iter = dagNode.find(containingTriangle);

	// Get the vertices of the parent triangle
	int pIndex1, pIndex2, pIndex3;
	containingTriangle.get(pIndex1, pIndex2, pIndex3);

	std::cout << "pIdx1-3=" << pIndex1 << "," << pIndex2 << "," << pIndex3 << ", pIdx=" << pIdx << std::endl;

	// Construct 3 Trirecords, one for each child triangle
	// and add it to existingChildren.
	TriRecord child1(pIndex1, pIndex2, pIdx);
	TriRecord child2(pIndex2, pIndex3, pIdx);
	TriRecord child3(pIndex3, pIndex1, pIdx);
	existingChildren.push_back(child1);
	existingChildren.push_back(child2);
	existingChildren.push_back(child3);

	std::cout << "addChildrenNodes 3" << std::endl;

	// Insert the parent-child relationship into the DAG, only if the containing triangle itself is not the bounding triangle.
	if (pIdx != pIndex1 && pIdx != pIndex2 && pIdx != pIndex3) {
		dagNode_.insert(map< TriRecord, vector<TriRecord> >::value_type(containingTriangle, existingChildren));
		leafNodeList_.push_back(child1);
		leafNodeList_.push_back(child2);
		leafNodeList_.push_back(child3);
	} else {
		leafNodeList_.push_back(containingTriangle);
	}

	// Push the parent node into the orderedKeyList.
	// This will preserve the order in which parents have been inserted.
	// First record will always be the bounding triangle, which we will use in
	// the `findLeafNodeforPoint()` for setting the initial worklist

	std::cout << "addChildrenNodes 4" << std::endl;

	orderedKeyList_.push_back(containingTriangle);

	// Section for removing the newly created parent node from the leafnodeList.
	for (vector<TriRecord>::iterator it = leafNodeList_.begin(); it != leafNodeList_.end(); ) {
		TriRecord tri = *it;
		MyPoint circumCntr1;
		if (tri == containingTriangle) {
			it = leafNodeList_.erase(it);

			// Find the circumcenter of this triangle
		} else {
			++it;
		}
	}

	std::cout << "addChildrenNodes 5" << std::endl;
}



// Please note that the edge to be flipped here is the 2nd and 3rd parameters.
void DirectedGraph::addFlipChildrenNodes(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
	assert(pIdx1 >= 1 && pIdx1 < 1000000); // XXX magic bound
	assert(pIdx2 >= 1 && pIdx2 < 1000000);
	assert(pIdx3 >= 1 && pIdx3 < 1000000);
	assert(pIdx4 >= 1 && pIdx4 < 1000000);

	vector<TriRecord> parentTriangles = findNodesForEdge(pIdx2, pIdx3);

	TriRecord newTri1(pIdx1, pIdx2, pIdx4);
	TriRecord newTri2(pIdx1, pIdx3, pIdx4);

	vector<TriRecord> children;
	children.push_back(newTri1);
	children.push_back(newTri2);

	// Add the new triangles into the leafnodeList
	leafNodeList_.push_back(newTri1);
	leafNodeList_.push_back(newTri2);

	for (vector<TriRecord>::iterator iter = parentTriangles.begin(); iter != parentTriangles.end(); ++iter) {
		TriRecord triangle = *iter;
		dagNode_.insert(map< TriRecord, vector<TriRecord> >::value_type(triangle, children));

		// Remove the new parents from leafnodeList
		for (vector<TriRecord>::iterator it = leafNodeList_.begin(); it != leafNodeList_.end(); ) {
			TriRecord tri = *it;

			if (tri == triangle) {
				it = leafNodeList_.erase(it);
			} else {
				++it;
			}
		}
	}
}



// Removes everything from DAG
void DirectedGraph::clearDirectedGraph() {
	dagNode_.clear();
	leafNodeList_.clear();
	orderedKeyList_.clear();
}

