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
	map< TriRecord, vector<TriRecord> >::iterator iter;
	vector<TriRecord> existingChildren;
	TriRecord child1, child2, child3;
	int pIndex1, pIndex2, pIndex3;

	//iter = dagNode.find(containingTriangle);

	// Get the vertices of the parent triangle
	pIndex1 = containingTriangle.vi_[0];
	pIndex2 = containingTriangle.vi_[1];
	pIndex3 = containingTriangle.vi_[2];

	// Construct 3 Trirecords, one for each child triangle and add it to existingChildren.
	child1.vi_[0] = pIndex1;
	child1.vi_[1] = pIndex2;
	child1.vi_[2] = pIdx;
	child2.vi_[0] = pIndex2;
	child2.vi_[1] = pIndex3;
	child2.vi_[2] = pIdx;
	child3.vi_[0] = pIndex3;
	child3.vi_[1] = pIndex1;
	child3.vi_[2] = pIdx;
	existingChildren.push_back(child1);
	existingChildren.push_back(child2);
	existingChildren.push_back(child3);

	// Insert the parent-child relationship into the DAG, only if the containing triangle itself is not the bounding triangle.
	if (pIdx != pIndex1 && pIdx != pIndex2 && pIdx != pIndex3) {
		dagNode_.insert(map< TriRecord, vector<TriRecord> >::value_type(containingTriangle, existingChildren));
		leafNodeList_.push_back(child1);
		leafNodeList_.push_back(child2);
		leafNodeList_.push_back(child3);
	} else
		leafNodeList_.push_back(containingTriangle);

	// Push the parent node into the orderedkeylist. This will preserve the order in which parents have been inserted.
	// First record will always be the bounding triangle, which we will use in the findLeafNodeforPoint for setting the
	// initial worklist


	orderedKeyList_.push_back(containingTriangle);

	// Section for removing the newly created parent node from the leafnodeList.
	for (vector<TriRecord>::iterator it = leafNodeList_.begin(); it != leafNodeList_.end(); ) {
		TriRecord tri = *it;
		MyPoint circumCntr1;
		if (tri.vi_[0] == containingTriangle.vi_[0] &&
		    tri.vi_[1] == containingTriangle.vi_[1] &&
		    tri.vi_[2] == containingTriangle.vi_[2]) {
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


	TriRecord rootNode;

	// If the root node has no edges, it is the bounding triangle.
	if (!orderedkeyList.empty()) {
		rootNode = orderedkeyList.front();

		// Happens when the ordered list only has bounding triangle
		if (dagNode.size() == 0) {
			worklist.push_back(rootNode);
		} else {
			worklist = dagNode.find(rootNode) ->second;
		}
	} else {
		// Return the trirecord composed of last 3 entries from myPoints pointset. The incoming point ID is the last vertex of
		// the bounding triangle.
		TriRecord boundingTri;
		boundingTri.vi_[0] = pIdx;
		boundingTri.vi_[1] = pIdx - 1;
		boundingTri.vi_[2] = pIdx - 2;

		return boundingTri;
	}

	// Iterate through the worklist and find the containing triangle. Once found, replace the worklist with
	// the new worklist of this triangle and continue iteration.


	for (vector<TriRecord>::iterator iter = worklist.begin(); iter != worklist.end(); ) {
		TriRecord checkTriangle = *iter;

		int pIndex1, pIndex2, pIndex3;
		pIndex1 = checkTriangle.vi_[0];
		pIndex2 = checkTriangle.vi_[1];
		pIndex3 = checkTriangle.vi_[2];


		int ret = triVertices_.inTri(pIndex1, pIndex2, pIndex3, pIdx); // If ret is >=0, the point is inside the triangle

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

		int pIndex1,pIndex2, pIndex3;
		pIndex1 = checkTriangle.vi_[0];
		pIndex2 = checkTriangle.vi_[1];
		pIndex3 = checkTriangle.vi_[2];

		if (pIndex1 == pIdx1 || pIndex2 == pIdx1 || pIndex3 == pIdx1) {
			if (pIndex1 == pIdx2 || pIndex2 == pIdx2 || pIndex3 == pIdx2) {
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

		int pIndex1,pIndex2, pIndex3;
		pIndex1 = checkTriangle.vi_[0];
		pIndex2 = checkTriangle.vi_[1];
		pIndex3 = checkTriangle.vi_[2];

		if (pIndex1 == pIdx1 || pIndex2 == pIdx1 || pIndex3 == pIdx1) {
			templist.push_back(checkTriangle);
		}
	}

	//
	TriRecord pickedTri = templist.front();
	TriRecord nextTri;
	TriRecord prevTri = templist.front();

	outputlist.push_back(pickedTri);
	int commonvert, nextvert;

	if (pIdx1 == pickedTri.vi_[0]) {
		commonvert = 0; nextvert = 1;
	} else if (pIdx1 == pickedTri.vi_[1]) {
		commonvert = 1; nextvert = 2;
	} else if (pIdx1 == pickedTri.vi_[2]) {
		commonvert = 2; nextvert = 0;
	}

	while (true) {
		vector<TriRecord> fnextlist = findNodesForEdge(prevTri.vi_[commonvert], prevTri.vi_[nextvert]);

		if (fnextlist.front().vi_[0] == prevTri.vi_[0] &&
		    fnextlist.front().vi_[1] == prevTri.vi_[1] &&
		    fnextlist.front().vi_[2] == prevTri.vi_[2]) {
		    nextTri = fnextlist.back();
		} else {
			nextTri = fnextlist.front();
		}

		// If the newly found triangle is the same as the very first triangle picked,
		// then a full cycle of sort has been completed.
		if (nextTri.vi_[0] == pickedTri.vi_[0] &&
		    nextTri.vi_[1] == pickedTri.vi_[1] &&
		    nextTri.vi_[2] == pickedTri.vi_[2])
			break;

		outputlist.push_back(nextTri);

		// Find an edge in nextTri that has the common vertex
		// but not shared with prevTri
		// XXX The following *badly* could be simplified w/ tmp bool vars.
		if ((nextTri.vi_[0] == prevTri.vi_[commonvert] &&
		     nextTri.vi_[1] != prevTri.vi_[nextvert]) ||
		    (nextTri.vi_[0] != prevTri.vi_[nextvert] &&
		     nextTri.vi_[1] == prevTri.vi_[commonvert])) {
			if (nextTri.vi_[0] == prevTri.vi_[commonvert] &&
			    nextTri.vi_[1] != prevTri.vi_[nextvert]) {
				nextvert   = 1;
				commonvert = 0;
			} else {
				nextvert   = 0;
				commonvert = 1;
			}
		} else if ((nextTri.vi_[1] == prevTri.vi_[commonvert] &&
		            nextTri.vi_[2] != prevTri.vi_[nextvert]) ||
		           (nextTri.vi_[1] != prevTri.vi_[nextvert] &&
		            nextTri.vi_[2] == prevTri.vi_[commonvert])) {
			if (nextTri.vi_[1] == prevTri.vi_[commonvert] &&
			    nextTri.vi_[2] != prevTri.vi_[nextvert]) {
				nextvert   = 2;
				commonvert = 1;
			} else {
				nextvert   = 1;
				commonvert = 2;
			}
		} else if ((nextTri.vi_[2] == prevTri.vi_[commonvert] &&
		            nextTri.vi_[0] != prevTri.vi_[nextvert]) ||
		           (nextTri.vi_[2] != prevTri.vi_[nextvert] &&
		            nextTri.vi_[0] == prevTri.vi_[commonvert])) {
			if (nextTri.vi_[2] == prevTri.vi_[commonvert] &&
			    nextTri.vi_[0] != prevTri.vi_[nextvert]) {
				nextvert   = 0;
				commonvert = 2;
			} else {
				nextvert   = 2;
				commonvert = 0;
			}
		}

		prevTri.vi_[0] = nextTri.vi_[0];
		prevTri.vi_[1] = nextTri.vi_[1];
		prevTri.vi_[2] = nextTri.vi_[2];
	}

	return outputlist;
}



// Please note that the edge to be flipped here is the 2nd and 3rd parameters.
void DirectedGraph::addFlipChildrenNodes(int pIdx1, int pIdx2, int pIdx3, int pIdx4) {
	vector<TriRecord> parentTriangles = findNodesForEdge(pIdx2, pIdx3);

	TriRecord newTri1;
	newTri1.vi_[0] = pIdx1;
	newTri1.vi_[1] = pIdx2;
	newTri1.vi_[2] = pIdx4;

	TriRecord newTri2;
	newTri2.vi_[0] = pIdx1;
	newTri2.vi_[1] = pIdx3;
	newTri2.vi_[2] = pIdx4;

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

			if (tri.vi_[0] == triangle.vi_[0] &&
			    tri.vi_[1] == triangle.vi_[1] &&
			    tri.vi_[2] == triangle.vi_[2]) {
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

