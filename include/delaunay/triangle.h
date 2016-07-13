#ifndef DELAUNAY_TRIANGLEH
#define DELAUNAY_TRIANGLEH

#include <stdlib.h>

#include <ostream>
#include <vector>



namespace delaunay {

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



	int vertexNotSharedWith(const TriRecord& other) {
		int pIdx1, pIdx2, pIdx3;
		other.get(pIdx1, pIdx2, pIdx3);
		if (!hasPointIndex(pIdx1)) {
			return pIdx1;
		} else if (!hasPointIndex(pIdx2)) {
			return pIdx2;
		} else if (!hasPointIndex(pIdx3)) {
			return pIdx3;
		} else {
			return 0;
		}
	}



	void set(int idx1, int idx2, int idx3) {
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
};

}

#endif
