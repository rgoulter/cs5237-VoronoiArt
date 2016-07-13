#include "delaunay/dagnode.h"

#include <algorithm>
#include <iostream>
#include <stack>

#include "delaunay/trianglegeometry.h"

using std::find;
using std::stack;
using std::vector;
using std::cout;
using std::endl;



namespace delaunay {

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

		if (node->isLeaf() &&
		    inTriangle(pointSet, node->tri_, pIdx1) >= 0 &&
		    inTriangle(pointSet, node->tri_, pIdx2) >= 0 &&
		    find(outputList.begin(), outputList.end(), node) == outputList.end()) {
			outputList.push_back(node);
		}

		for (vector<DAGNode*>::iterator iter = node->children_.begin();
		     iter != node->children_.end();
		     ++iter) {
			DAGNode *childNode = *iter;

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

}

