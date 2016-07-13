#ifndef DELAUNAY_DAGNODEH
#define DELAUNAY_DAGNODEH

#include <vector>

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"
#include "delaunay/triangulation.h"



namespace delaunay {

class DAGNode {
public:
	static std::vector<DAGNode*> leafNodesContainingPoint(DAGNode* root, const PointSetArray& pointSet, int pIdx);

	/// 'contains edge' iff 'contains both points'.
	/// Order of points not considered.
	static std::vector<DAGNode*> leafNodesContainingEdge(DAGNode* root, const PointSetArray& pointSet, int pIdx1, int pIdx2);

	static int findAdjacentTriangle(DAGNode* root, const PointSetArray& pointSet, int pIdx1, int pIdx2, int pIdx3);

	DAGNode(const TriRecord& tri) : tri_(tri) {}

	bool isLeaf() const { return children_.empty(); }

	TriRecord tri_;
	std::vector<DAGNode *> children_;
	FIndex fIndex_;
};

}

#endif
