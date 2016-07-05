#include "trist.h"



TriRecord::TriRecord(int idx1, int idx2, int idx3) {
	vi_[0] = idx1;
	vi_[1] = idx2;
	vi_[2] = idx3;
}



void TriRecord::nextEdge(int& commonIdx, int& nextIdx, const TriRecord& nextTri) const {
	int commonVert = vi_[commonIdx];
	int notVert    = vi_[nextIdx]; // don't want nextIdx

	if (nextTri.vi_[0] == commonVert &&
		nextTri.vi_[1] != notVert) {
		commonIdx = 0;
		nextIdx   = 1;
	} else if (nextTri.vi_[0] != notVert &&
	           nextTri.vi_[1] == commonVert) {
		commonIdx = 1;
		nextIdx   = 0;
	} else if (nextTri.vi_[1] == commonVert &&
	           nextTri.vi_[2] != notVert) {
		commonIdx = 1;
		nextIdx   = 2;
	} else if (nextTri.vi_[1] != notVert &&
	           nextTri.vi_[2] == commonVert) {
		commonIdx = 2;
		nextIdx   = 1;
	} else if (nextTri.vi_[2] == commonVert &&
	           nextTri.vi_[0] != notVert) {
		commonIdx = 2;
		nextIdx   = 0;
	} else if (nextTri.vi_[2] != notVert &&
	           nextTri.vi_[0] == commonVert) {
		commonIdx = 0;
		nextIdx   = 2;
	}
}



Trist::Trist() {
}



int Trist::noTri() {
	return triPoints.size();
}



// TODO `makeTri` vs TriRecord c'tor. esp. use of `fnext_`
int Trist::makeTri(int pIndex1, int pIndex2, int pIndex3, bool autoMerge) {
	// Add a triangle into the Trist with the three point indices
	// Moreover, automatically establish the fnext pointers to its neigbhours if autoMerge = true

	TriRecord record(pIndex1, pIndex2, pIndex3);

	// TODO: Allow for auto merging.
	record.fnext_[0] = -1;
	record.fnext_[1] = -1;
	record.fnext_[2] = -1;
	record.fnext_[3] = -1;
	record.fnext_[4] = -1;
	record.fnext_[5] = -1;

	triPoints.push_back(record);
	return (triPoints.size());
}



void Trist::delTri(OrTri ef) {
	// Delete a triangle, but you can assume that this is ONLY used by the IP operation
	// You may want to make sure all its neighbours are detached (below)

	// This removes the tri from the triPoints table by swapping it with the last item,
	//  and then removing it (since that's easier).
	// Ultimately, doing in this way CHANGES TRIANGLE INDEXES.. which means we can't quite
	//  keep OrTri values outside of Trist.
	// An alternative might be to just "mark as deleted", and keep track of num_triangles as
	//  an additional variable.

	fdetach(ef);

	int tIdx = (ef >> 3);
	int last = triPoints.size() - 1;
	if (tIdx < last) {
		// If ef isn't the last triangle..
		relableRefsToTri((OrTri) (last << 3), ef);
		triPoints[tIdx] = triPoints[last];
	}

	triPoints.pop_back();
}



OrTri Trist::enext(OrTri ef) {
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	return (OrTri) (tId << 3) + kEdgeNext[v];
}



OrTri Trist::sym(OrTri ef) {
	// sym cycles  i <> i + 3 % 6
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	return (OrTri) (tId << 3) + (v + 3 % 6);
}



OrTri Trist::fnext(OrTri ef) {
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	// Get the tIdth triangle record.
	TriRecord record = triPoints[tId];

	return (OrTri) (tId << 3) + (record.fnext_[v]);
}



void Trist::getVertexIdx(OrTri ef, int& pIdx1, int& pIdx2, int& pIdx3) {
	// return the three indices of the three vertices by OrTri
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	pIdx1 = record.vi_[0];
	pIdx2 = record.vi_[1];
	pIdx3 = record.vi_[2];
}



int Trist::org(OrTri ef) {
	// the index of the first vertex of OrTri, e.g. org(bcd) => b
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	// See formula in trist.h
	return (v < 3) ? v : (v + 1) % 3;
}



int Trist::dest(OrTri ef) {
	// the index of the second vertex of OrTri, e.g. org(bcd) => c
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	// See formula in trist.h
	return (v < 3) ? (v + 1) % 3 : v;
}



void Trist::fmerge(OrTri abc, OrTri abd) {
	// glue two neighbouring triangles, result abd = fnext(abc)
	// ASSUME 2D ONLY
	// This assumption helps us with gluing them together, since we
	//  don't need to search for the next triangles in general.

	// Here we DO NOT glue the symmetric triangles together. Because.

	int abcIdx = abc >> 3;
	int abcV = abc & 7;
	int abdIdx = abd >> 3;
	int abdV = abd & 7;

	// 2D only
	triPoints[abcIdx].fnext_[abcV] = abd; // We record the OrdTri in fnext_.
	triPoints[abdIdx].fnext_[abdV] = abc; // We record the OrdTri in fnext_.
}



void Trist::relableRefsToTri(OrTri oldEF, OrTri newEF) {
	// Slight misnomer; this renames
	// ASSUME 2D ONLY
	// this assumption allows us to just not give a toss.

	OrTri t = oldEF;
	int tIdx, v;

	tIdx = t >> 3;
	v = t & 7;

	TriRecord *r = &(triPoints[tIdx]);

	// Check all the triangles the tri oldEF is connected to,
	// and then set their references to oldEF to point to newEF.
	for (v = 0; v < 6; v++){
		if (r->fnext_[v] >= 0){
			int nextTIdx = (r->fnext_[v] >> 3);
			int nextV = (r->fnext_[v] & 7);
			TriRecord *nextT = &(triPoints[nextTIdx]);

			// Assumes 2D. Relabel
			nextT->fnext_[nextV] = newEF;
		}
	}
}


void Trist::fdetach(OrTri abc) {
	// detach triangle abc with all its neighbours (undo fmerge)
	relableRefsToTri(abc, -1);

	TriRecord *r = &(triPoints[abc >> 3]);
	r->fnext_[0] = -1;
	r->fnext_[1] = -1;
	r->fnext_[2] = -1;
	r->fnext_[3] = -1;
	r->fnext_[4] = -1;
	r->fnext_[5] = -1;
}



void Trist::eraseAllTriangles() {
	triPoints.clear();
}



void Trist::incidentTriangles(int ptIndex, int& noOrTri, OrTri* otList) {
	// A suggested function: you may want this function to return all the OrTri
	// that are incident to this point
	// Ignore this if you don't feel a need

	noOrTri = 0;

	for (unsigned int i = 0; i < triPoints.size(); i++) {
		TriRecord *r = &(triPoints[i]);

		if (r->vi_[0] == ptIndex || r->vi_[1] == ptIndex || r->vi_[2] == ptIndex) {
			noOrTri = noOrTri + 1;
			otList = (OrTri*) realloc(otList, noOrTri * sizeof(OrTri));

			otList[noOrTri - 1] = i;
		}
	}
}

