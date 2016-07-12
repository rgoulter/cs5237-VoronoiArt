#ifndef DELAUNAY_TRIANGLEH
#define DELAUNAY_TRIANGLEH

#include <assert.h>
#include <stdlib.h>

#include <ostream>
#include <vector>

#include "delaunay/pointsetarray.h"



namespace delaunay {

/// The OrTri data structure for an Oriented Triangle
typedef  int OrTri;

/// The index of a triangle Hint: NOT a triangle if it's negative
/// You should be able to make all the triangle indices to be from 0 to n - 1 (n = number of triangles)
typedef  int FIndex;


class Trist;



/// Method to add new children triangles to a parent triangle.
/// Call this from tryInsertPoint method in main.cpp
void findBoundingTri(PointSetArray &);



class TriRecord {
public:
	TriRecord(int idx1, int idx2, int idx3);



	// Benny: override < operator for the purpose of using a map in directedGraph.cpp.
	bool operator<(const TriRecord di) const {
		if (vi_[0] < di.vi_[0])  return true;
		if (vi_[0] > di.vi_[0])  return false;
		// Otherwise a are equal
		if (vi_[1] < di.vi_[1])  return true;
		if (vi_[1] > di.vi_[1])  return false;
		// Otherwise both are equal
		if (vi_[2] < di.vi_[2])  return true;
		if (vi_[2] > di.vi_[2])  return false;

		return false;
	}



	bool operator==(const TriRecord& di) const {
		return vi_[0] == di.vi_[0] &&
		       vi_[1] == di.vi_[1] &&
		       vi_[2] == di.vi_[2];
	}



	TriRecord& operator=(const TriRecord& rhs) {
		if (this != &rhs) {
			vi_[0] = rhs.vi_[0];
			vi_[1] = rhs.vi_[1];
			vi_[2] = rhs.vi_[2];

			// Do we need to worry about fnext_?
		}

		return *this;
	}



	bool hasPointIndex(int pIdx) const {
		return vi_[0] == pIdx ||
		       vi_[1] == pIdx ||
		       vi_[2] == pIdx;
	}



	/// Looser equality than === sign.
	/// Tri(1,2,3) `sameVertices` Tri(3,2,1) is `true`.
	bool sameVertices(const TriRecord& di) const {
		return hasPointIndex(di.vi_[0]) &&
		       hasPointIndex(di.vi_[1]) &&
		       hasPointIndex(di.vi_[2]);
	}



	void get(int& idx1, int& idx2, int& idx3) const {
		idx1 = vi_[0];
		idx2 = vi_[1];
		idx3 = vi_[2];
	}



	// Sometimes do need to access `vi_`.
	int pointIndexOf(int vertIdx) const {
		return vi_[vertIdx];
	}



	void set(int idx1, int idx2, int idx3) {
		assert(idx1 >= 0 && idx1 < 1000000);
		assert(idx2 >= 0 && idx2 < 1000000);
		assert(idx3 >= 0 && idx3 < 1000000);
		vi_[0] = idx1;
		vi_[1] = idx2;
		vi_[2] = idx3;
	}



	/// Find an edge in nextTri that has the common vertex
	/// but not shared with prevTri
	void nextEdge(int& commonIdx, int& notIdx, const TriRecord& nextTri) const;



	friend std::ostream& operator<<(std::ostream& os, const TriRecord& tri);



private:
	int vi_[3];

	OrTri fnext_[6];

friend class Trist;
};



bool isTriangleCCW(const PointSetArray& psa, const TriRecord& tri);



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri, int pIdx1, int pIdx2);



bool intersectsTriangle(const PointSetArray& psa, const TriRecord& tri1, const TriRecord& tri2);



}

#endif
