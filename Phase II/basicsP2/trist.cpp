#include "trist.h"


Trist::Trist()
{
	en_[0] = 1;
	en_[1] = 2;
	en_[2] = 0;
	en_[3] = 5;
	en_[4] = 3;
	en_[5] = 4;

}



int Trist::noTri(){
	return triPoints.size();
}



int Trist::makeTri(int pIndex1, int pIndex2, int pIndex3, bool autoMerge = false){
	// Add a triangle into the Trist with the three point indices
	// Moreover, automatically establish the fnext pointers to its neigbhours if autoMerge = true

	TriRecord record;
	record.vi_[0] = pIndex1;
	record.vi_[1] = pIndex2;
	record.vi_[2] = pIndex3;

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



void Trist::delTri(OrTri oti){
	// Delete a triangle, but you can assume that this is ONLY used by the IP operation
	// You may want to make sure all its neighbours are detached (below)

	// TODO: If we delete, then we will have to ensure triangle indices are valid afterwards.
	// Perhaps better to just ignore? Otherwise quite expensive.
}



OrTri Trist::enext(OrTri ef){
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	return (OrTri) (tId << 3) + en_[v];
}



OrTri Trist::sym(OrTri ef){
	// sym cycles  i <> i + 3 % 6
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	return (OrTri) (tId << 3) + (v + 3 % 6);
}



OrTri Trist::fnext(OrTri ef){
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	// Get the tIdth triangle record.
	TriRecord record = triPoints[tId];

	return (OrTri) (tId << 3) + (record.fnext_[v]);
}



void Trist::getVertexIdx(OrTri ef, int& pIdx1, int& pIdx2, int& pIdx3){
	// return the three indices of the three vertices by OrTri
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];
	
	pIdx1 = record.vi_[0];
	pIdx2 = record.vi_[1];
	pIdx3 = record.vi_[2];
}



int Trist::org(OrTri ef){
	// the index of the first vertex of OrTri, e.g. org(bcd) => b
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	// See formula in trist.h
	int i = (v < 3) ? v : (v + 1) % 3;
}



int Trist::dest(OrTri ef){
	// the index of the second vertex of OrTri, e.g. org(bcd) => c
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	// See formula in trist.h
	int i = (v < 3) ? (v + 1) % 3 : v;
}



void Trist::fmerge(OrTri abc, OrTri abd){
	// glue two neighbouring triangles, result abd = fnext(abc)
	// ASSUME 2D ONLY
}



void Trist::fdetach(OrTri abc){
	// detach triangle abc with all its neighbours (undo fmerge)
	// ASSUME 2D ONLY
}



void Trist::incidentTriangles(int ptIndex, int& noOrTri, OrTri* otList){
	// A suggested function: you may want this function to return all the OrTri
	// that are incident to this point
	// Ignore this if you don't feel a need

}
