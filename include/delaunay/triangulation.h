#ifndef DELAUNAY_TRIANGULATIONH
#define DELAUNAY_TRIANGULATIONH

#include <ostream>
#include <vector>

#include "delaunay/pointsetarray.h"
#include "delaunay/triangle.h"

#ifndef NDEBUG
#define TRIANGULATION_CHECK
#endif



namespace delaunay {

/// The triangle indices are from
/// 1 to n (n = number of triangles)
typedef int FIndex;



/*

  For a triangle abc, if version 1 is abc

  version 0 abc     (v:012)
  version 1 bca     (v:120)
  version 2 cab     (v:201)
  version 3 bac     (v:102)
  version 4 cba     (v:210)
  version 5 acb     (v:021)

  enext cycles   0 > 1 > 2 > 0 ,  5 > 4 > 3 > 5
  sym cycles  i <> i + 3 % 6

  org  = ver < 3 ? v[ver] : v[(ver+1)%3]
  dest = ver < 3 ? v[(ver+1)%3] : v[ver-3]

*/
const int kEdgeNext[6] = { 1, 2, 0, 5, 3, 4 };



class LinkedTriangle {
public:
	LinkedTriangle(const TriRecord& tri) : tri_(tri) {
		links_[0] = 0;
		links_[1] = 0;
		links_[2] = 0;
	}

	void getEdgeIndices(int iIndex, int& edgeIJ, int& edgeJK, int& edgeKI) const;

	TriRecord tri_;

	FIndex links_[3];
};



class Triangulation {
public:
	Triangulation();
	~Triangulation();

	FIndex addLinkedTri(const TriRecord& tri);

	/// Merely remove the triangle, don't unlink it.
	void removeLinkedTri(FIndex triIdx);

	bool isLinkedTri(FIndex idx) const {
		return 1 <= idx &&
		       idx <= linkedTriangles_.size() &&
		       linkedTriangles_[idx - 1] != nullptr;
	}

	const LinkedTriangle* operator[](FIndex idx) const;

	LinkedTriangle* operator[](FIndex idx);

	int size() const { return linkedTriangles_.size(); }

	/// Both triIJK, otherTri need to be linked-triangles in this triangulation.
	void setLink(FIndex triIJK, int edgeIdx, FIndex otherTri, int otherEdgeIdx);

	/// triIJK needs to be a linked-triangle in this triangulation,
	/// otherTri does not.
	void setLink(FIndex triIJK, int edgeIdx, FIndex otherTri);

	/// Return a vector of the existing LinkedTriangles.
	/// O(n), for n triangles added to / removed from the triangulation.
	std::vector<FIndex> getLinkedTriangles() const;

	bool checkConsistent() const;

private:
	std::vector<LinkedTriangle*> linkedTriangles_;

};



bool hasValidLinks(const Triangulation& trist, const LinkedTriangle* ltri);

}

#endif
