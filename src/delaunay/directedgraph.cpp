#include "directedgraph.h"

#include <iostream>

using std::map;
using std::vector;



DirectedGraph::DirectedGraph(PointSetArray pSet) {
	triVertices_ = pSet;
}



vector<TriRecord> DirectedGraph::getLeafNodes() {
	return leafNodeList_;
}



// This method adds children nodes to the specified parent node. This is only for new point additions and not for flipping.
void DirectedGraph::addChildrenNodes(int pIdx) {
	std::cout << "inside addchildrennodes:" << pIdx << std::endl;

	// Find triangle to which the point belongs. This function returns the bounding triangle in the case that the
	// point passed here is the end vertex of the bounding triangle
	TriRecord containingTriangle = findLeafNodeForPoint(pIdx);


	// Declare iterator for the dagNode, and the vector of TriRecords for the children of the node.
	// map< TriRecord, vector<TriRecord> >::iterator iter;
	vector<TriRecord> existingChildren;

	// iter = dagNode.find(containingTriangle);

	// Get the vertices of the parent triangle
	int pIndex1, pIndex2, pIndex3;
	containingTriangle.get(pIndex1, pIndex2, pIndex3);

	// Construct 3 Trirecords, one for each child triangle
	// and add it to existingChildren.
	TriRecord child1(pIndex1, pIndex2, pIdx);
	TriRecord child2(pIndex2, pIndex3, pIdx);
	TriRecord child3(pIndex3, pIndex1, pIdx);
	existingChildren.push_back(child1);
	existingChildren.push_back(child2);
	existingChildren.push_back(child3);

	// Insert the parent-child relationship into the DAG, only if the containing triangle itself is not the bounding triangle.
	if (pIdx != pIndex1 && pIdx != pIndex2 && pIdx != pIndex3) {
		dagNode_.insert(map< TriRecord, vector<TriRecord> >::value_type(containingTriangle, existingChildren));
		leafNodeList_.push_back(child1);
		leafNodeList_.push_back(child2);
		leafNodeList_.push_back(child3);
	} else {
		leafNodeList_.push_back(containingTriangle);
	}

	// Push the parent node into the orderedkeylist. This will preserve the order in which parents have been inserted.
	// First record will always be the bounding triangle, which we will use in the findLeafNodeforPoint for setting the
	// initial worklist


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
}



// This method returns the triangle which encloses the input point.
TriRecord DirectedGraph::findLeafNodeForPoint(int pIdx) {
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


	for (vector<TriRecord>::iterator iter = worklist.begin(); iter != worklist.end(); ) {
		TriRecord checkTriangle = *iter;

		// TODO inTri would benefit from using TriRecord
		int pIndex1, pIndex2, pIndex3;
		checkTriangle.get(pIndex1, pIndex2, pIndex3);

		int ret = triVertices_.inTri(pIndex1, pIndex2, pIndex3, pIdx);

		// If ret is >=0, the point is inside the triangle
		if (ret >= 0) {
			if (dagNode_.find(checkTriangle) != dagNode_.end()) {
				// There could be error here.
				// Does C++ allow us to change the object we are iterating through?
				worklist = dagNode_.find(checkTriangle)->second;
				iter = worklist.begin();
			} else {
				return checkTriangle;
			}
		} else {
			++iter;
		}
	}
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



// This method returns the set of triangles the input point belongs to.
vector<TriRecord> DirectedGraph::findLinkedNodes(int pIdx1) {
	vector<TriRecord> templist, outputlist;

	for (vector<TriRecord>::iterator iter = leafNodeList_.begin(); iter != leafNodeList_.end(); ++iter) {
		TriRecord checkTriangle = *iter;

		// trirecord has pIdx1?
		if (checkTriangle.hasPointIndex(pIdx1)) {
			templist.push_back(checkTriangle);
		}
	}

	//
	TriRecord pickedTri = templist.front();
	TriRecord prevTri = templist.front();

	outputlist.push_back(pickedTri);
	int commonvert;
	int nextvert;

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

	while (true) {
		vector<TriRecord> fnextlist = findNodesForEdge(prevTri.pointIndexOf(commonvert), prevTri.pointIndexOf(nextvert));

		TriRecord nextTri =
			(fnextlist.front() == prevTri) ?
			fnextlist.back() :
			fnextlist.front();

		// If the newly found triangle is the same as
		//    the very first triangle picked,
		// then a full cycle of sort has been completed.
		if (nextTri == pickedTri)
			break;

		outputlist.push_back(nextTri);

		// Find an edge in nextTri that has the common vertex
		// but not shared with prevTri
		prevTri.nextEdge(commonvert, nextvert, nextTri);

		prevTri = nextTri;
	}

	return outputlist;
}



// Please note that the edge to be flipped here is the 2nd and 3rd parameters.
void DirectedGraph::addFlipChildrenNodes(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
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

