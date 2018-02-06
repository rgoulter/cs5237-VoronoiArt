#include "delaunay/dagnode.h"

#include <algorithm>
#include <iostream>
#include <stack>

#include "delaunay/trianglegeometry.h"

#include "tracing.h"

using std::find;
using std::shared_ptr;
using std::stack;
using std::vector;
using std::cout;
using std::endl;



namespace delaunay {

// O(log n) impl.
vector<shared_ptr<DAGNode>> DAGNode::leafNodesContainingPoint(shared_ptr<DAGNode> root, const PointSetArray& pointSet, int pIdx) {
	TRACE("[dagNode.leafNodesContainingPoint(root, psa, pointIndex=" << pIdx << ")]");
	vector<shared_ptr<DAGNode>> outputList;

	stack<shared_ptr<DAGNode>> stk;
	stk.push(root);

	while (!stk.empty()) {
		shared_ptr<DAGNode> node = stk.top();
		stk.pop();

		if (node->isLeaf()) {
			outputList.push_back(node);
		}

		for (shared_ptr<DAGNode> childNode : node->children_) {
			int p1Idx, p2Idx, p3Idx;
			childNode->tri_.get(p1Idx, p2Idx, p3Idx);

			// TODO convenient if PSet.inTri used TriRecord
			if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx) > 0) {
				stk.push(childNode);
			} else if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx) == 0) {
				// cout << "DAG.addVertex, CASE: pIdx *on* tri." << endl;
				stk.push(childNode);
			}
		}
	}

	return outputList;
}



vector<shared_ptr<DAGNode>> DAGNode::leafNodesContainingEdge(shared_ptr<DAGNode> root, const PointSetArray& pointSet, int pIdx1, int pIdx2) {
	TRACE("[dagNode.leafNodesContainingEdge(root, psa, pointIndex1=" << pIdx1 << ", pointIndex2=" << pIdx2 << ")]");

	vector<shared_ptr<DAGNode>> outputList;

	stack<shared_ptr<DAGNode>> stk;
	stk.push(root);

	while (!stk.empty()) {
		shared_ptr<DAGNode> node = stk.top();
		stk.pop();

		if (node->isLeaf() &&
		    inTriangle(pointSet, node->tri_, pIdx1) >= 0 &&
		    inTriangle(pointSet, node->tri_, pIdx2) >= 0 &&
		    find(outputList.begin(), outputList.end(), node) == outputList.end()) {
			outputList.push_back(node);
		}

		for (shared_ptr<DAGNode> childNode : node->children_) {
			int p1Idx, p2Idx, p3Idx;
			childNode->tri_.get(p1Idx, p2Idx, p3Idx);

			// TODO convenient if PSet.inTri used TriRecord
			if (pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx1) >= 0 ||
				pointSet.inTri(p1Idx, p2Idx, p3Idx, pIdx2) >= 0) {
				stk.push(childNode);
			}
		}
	}

	return outputList;
}



int DAGNode::findAdjacentTriangle(shared_ptr<DAGNode> root, const PointSetArray& pointSet, int pIdx1, int pIdx2, int pIdx3) {
	TriRecord outerTri(pIdx1, pIdx2, pIdx3);
	vector<shared_ptr<DAGNode>> nodes = DAGNode::leafNodesContainingEdge(root, pointSet, pIdx2, pIdx3);

	for (const shared_ptr<DAGNode>& node : nodes) {
		const TriRecord& tri = node->tri_;

		int pIdx = outerTri.vertexNotSharedWith(tri);

		if (pIdx > 0) {
			return pIdx;
		}
	}

	return 0;
}

}

