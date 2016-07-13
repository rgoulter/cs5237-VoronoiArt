#include "delaunay/triangulation.h"

#include <assert.h>

#include <iostream>

#include "delaunay/li.h"
#include "delaunay/pointsetarray.h"

using std::cout;
using std::endl;



namespace delaunay {

void LinkedTriangle::getEdgeIndices(int iIndex, int& edgeIJ, int& edgeJK, int& edgeKI) const {
	int pIdx1, pIdx2, pIdx3;
	tri_.get(pIdx1, pIdx2, pIdx3);

	if (iIndex == pIdx1) {
		edgeIJ = 0;
		edgeJK = 1;
		edgeKI = 2;
	} else if (iIndex == pIdx2) {
		edgeIJ = 1;
		edgeJK = 2;
		edgeKI = 0;
	} else if (iIndex == pIdx3) {
		edgeIJ = 2;
		edgeJK = 0;
		edgeKI = 1;
	} else {
		edgeIJ = -1;
		edgeJK = -1;
		edgeKI = -1;
	}
}



Triangulation::Triangulation() {
}



FIndex Triangulation::addLinkedTri(const TriRecord& tri) {
	LinkedTriangle ltri(tri);
	linkedTriangles_.push_back(ltri);

	// FIndex starts from 1.
	return linkedTriangles_.size();
}



void Triangulation::setLink(FIndex triIJK, int edgeIdx, FIndex otherTri, int otherEdgeIdx) {
	assert(isLinkedTri(triIJK));
	assert(isLinkedTri(otherTri));

	LinkedTriangle& ltriIJK   = linkedTriangles_[triIJK - 1];
	ltriIJK.links_[edgeIdx] = otherTri;

	LinkedTriangle& ltriOther = linkedTriangles_[otherTri - 1];
	ltriOther.links_[otherEdgeIdx] = triIJK;
}



// triIJK should be an extant tri; otherTri doesn't need to be.
void Triangulation::setLink(FIndex triIJK, int edgeIdx, FIndex otherTri) {
	if (!isLinkedTri(otherTri)) {
		return;
	}

	// find the index of i,j,k
	LinkedTriangle ltriIJK = linkedTriangles_[triIJK - 1];
	int iIdx, jIdx, kIdx;
	ltriIJK.tri_.get(iIdx, jIdx, kIdx);


	if (edgeIdx == 0) {
		// otherTri shares edge IJ
		LinkedTriangle ltriXJI = linkedTriangles_[otherTri - 1];
		int edgeIdxJI, edgeIdxIX, edgeIdxXJ;
		ltriXJI.getEdgeIndices(jIdx, edgeIdxJI, edgeIdxIX, edgeIdxXJ);

		setLink(triIJK, edgeIdx, otherTri, edgeIdxJI);
	} else if (edgeIdx == 1) {
		// otherTri shares edge JK
		LinkedTriangle ltriXKJ = linkedTriangles_[otherTri - 1];
		int edgeIdxKJ, edgeIdxJX, edgeIdxXK;
		ltriXKJ.getEdgeIndices(kIdx, edgeIdxKJ, edgeIdxJX, edgeIdxXK);

		setLink(triIJK, edgeIdx, otherTri, edgeIdxKJ);
	} else if (edgeIdx == 2) {
		// otherTri shares edge KI
		LinkedTriangle ltriXIK = linkedTriangles_[otherTri - 1];
		int edgeIdxIK, edgeIdxKX, edgeIdxXI;
		ltriXIK.getEdgeIndices(iIdx, edgeIdxIK, edgeIdxKX, edgeIdxXI);

		setLink(triIJK, edgeIdx, otherTri, edgeIdxIK);
	}
}



const LinkedTriangle& Triangulation::operator[](FIndex idx) const {
	assert(1 <= idx && idx <= linkedTriangles_.size());

	return linkedTriangles_[idx - 1];
}



LinkedTriangle& Triangulation::operator[](FIndex idx) {
	assert(1 <= idx && idx <= linkedTriangles_.size());

	return linkedTriangles_[idx - 1];
}

}

