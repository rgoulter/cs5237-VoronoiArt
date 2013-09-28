#include "trist.h"
#include <assert.h>
#include <iostream> //remove later
#include <tuple>

//helper functions, to move.
std::tuple<int, int> Trist::ActiveEdge(OrTri x)
{
	//std::cout<<"sym("<<x<<") = ";
	bool swapped = false;
	if((x&7)>=3)
	{
		swapped = true;
		x = sym(x);
	}
	//std::cout<<x<<std::endl;
	int tid = x>>3;
	int version = x&7;
	int p1, p2;
	TriRecord currRec = triPoints[tid];
	p1 = currRec.vi_[version%3];
	p2 = currRec.vi_[(version+1)%3];
	if(swapped)
	{
		int temp = p1;
		p1 = p2;
		p2 = temp;
	}
	//std::cout<<"active edge of id="<<x<<"("<<currRec.vi_[0]<<", "<<currRec.vi_[1]<<", "<<currRec.vi_[2] <<") is ("<<p1<<", "<<p2<<")"<<std::endl;
	return std::make_tuple(p1, p2);
}

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



int Trist::makeTri(int pIndex1, int pIndex2, int pIndex3, bool autoMerge){
	// Add a triangle into the Trist with the three point indices
	// Moreover, automatically establish the fnext pointers to its neigbhours if autoMerge = true

	TriRecord record;
	record.vi_[0] = pIndex1;
	record.vi_[1] = pIndex2;
	record.vi_[2] = pIndex3;

	// TODO: Allow for auto merging.
	if(autoMerge)
	{
		//note that the edge in the +3 is the reverse direction and this forms a doubly linked list.
		//for each edge
		for(int i = 0;i<3;i++)
		{
			//Assume 0-indexed
			int tid = triPoints.size();
			int p1 = i%3;
			int p2 = (i+1)%3;
			int opposite = (i+2)%3;
			TriEdge currEdge = TriEdge(record.vi_[p1], record.vi_[p2]);
			OrTri currversion = (tid<<3) + i;
			//check if exist another triangle with that edge.
			std::unordered_map<TriEdge, OrTri>::const_iterator triIter = triAdj.find(currEdge);
			std::unordered_map<TriEdge, OrTri>::const_iterator t = triAdj.end();
			int last = 0;
			if(triIter != triAdj.end())
			{
			//exists:
				//THIS ONLY WORKS FOR 2D. Since in 2d, each edge is shared by at most 2 triangles... (otherwise orientation will be gone)
				//i just append it to the linked_list
				OrTri Xtri = triAdj[currEdge];
				int Xid = Xtri >> 3;
				int fversion = Xtri & 7; // ef & 00111b ... last 3 bits.
				//int rversion = (fversion>3)? fversion-3: fversion+3;
				TriRecord Xrecord = triPoints[Xid];

				//determine if this triangle uses the same orientation as us
				int Xp1, Xp2;
				std::tuple<int, int> Xedge = ActiveEdge(Xtri);
				Xp1 = std::get<0>(Xedge);
				Xp2 = std::get<1>(Xedge);
				bool reversed; //not sure whether i need it or not
				if(Xp1 == record.vi_[p1] && Xp2 == record.vi_[p2])
				{
					reversed = false;
				}
#ifndef NDEBUG
				else if (Xp2 == record.vi_[p1] && Xp1 == record.vi_[p2])
				{
					reversed = true;
					currversion = sym(currversion);
				}
				else
					assert(false); //why have i pulled this record if it were not matching.
#else
				else
				{
					reversed = true;
					currversion = sym(currversion);
				}
#endif

				OrTri Ytri = Xrecord.fnext_[fversion];
				if(Ytri != -1)
				{
					//NOTE: this code should be UNTESTED until 3d stuff is done.
					assert(false);

					last = 1;
				int Yid = Ytri >> 3;
				int rversion = sym(Ytri) & 7;
				TriRecord Yrecord = triPoints[Yid];
#ifndef NDEBUG
				
				//sanity check: is a linked list
				assert (sym(Yrecord.fnext_[rversion]) == Xtri);
#endif		
				//simply grab the (oriented) triangle and link it in - triangles are oriented on which edge they rotate on
				//before:
				// X -> Y
				// sym(Y) -> sym(X)
				//after:
				// X -> this -> Y
				// sym(y) -> sym(this) -> sym(X)
				
				// X -> this -> Y
				record.fnext_[i] = Ytri;
				Xrecord.fnext_[fversion] = currversion;
				
				// sym(y) -> sym(this) -> sym(X)
				record.fnext_[i+3] = sym(Xtri);
				Yrecord.fnext_[rversion] = sym(currversion);

				}
				else //Yrecord = -1. I.e. some other triangle exists but its the only one.
				{
#ifndef NDEBUG
					last = 2;
					assert(Xrecord.fnext_[sym(Xtri)&7] == -1); //if forward is -1, so must the reverse
#endif
					//then set both sides to this triangle, but with different orientations.
					Xrecord.fnext_[Xtri & 7] = currversion;
					Xrecord.fnext_[sym(Xtri) & 7] = sym(currversion);
					record.fnext_[currversion&7] = Xtri;
					record.fnext_[sym(currversion)&7] = sym(Xtri);

					triPoints[Xid] = Xrecord; // this is hack, i'm not sure why it's not updating without this. TODO: find out why.
					OrTri next = record.fnext_[currversion&7];
					OrTri prev = record.fnext_[sym(currversion)&7];
					if(next!=-1)
					{
					TriRecord nextrec = triPoints[next>>3];
					assert(nextrec.fnext_[sym(next)&7] == sym(currversion));
					TriRecord prevrec = triPoints[prev>>3];
					int p = sym(prev);
					int t = sym(prev)&7;
					assert(prevrec.fnext_[sym(prev)&7] == currversion);
					}
				}
			}
			else
			{
				last = 3;
			//no exists:
				//set both edges to -1
				record.fnext_[i] = -1;
				record.fnext_[i+3] = -1;
				//write this triangle into the edge table.
				triAdj[currEdge] = (tid<<3) + i; //pushed at end, assume zero indexed.
			}
#ifndef NDEBUG
			OrTri next = record.fnext_[currversion&7];
			OrTri prev = record.fnext_[sym(currversion)&7];
			if(next!=-1)
			{
			TriRecord nextrec = triPoints[next>>3];
			assert(nextrec.fnext_[sym(next)&7] == sym(currversion));
			TriRecord prevrec = triPoints[prev>>3];
			assert(prevrec.fnext_[sym(prev)&7] == currversion);
			}
#endif
		}
	}
	else
	{
		record.fnext_[0] = -1;
		record.fnext_[1] = -1;
		record.fnext_[2] = -1;
		record.fnext_[3] = -1;
		record.fnext_[4] = -1;
		record.fnext_[5] = -1;
	}
#ifndef NDEBUG
	//links are reasonable
	for(int i = 0;i<6;i++)
	{
		assert(record.fnext_[i]>=-1);
		OrTri next = record.fnext_[i];
		if(next != -1)
		{
			TriRecord nextrec = triPoints[next>>3];
			int tid = triPoints.size();
			/*std::cout<<(nextrec.fnext_[sym(next)&7])<<std::endl;
			std::cout<<(sym((tid<<3)+i))<<std::endl;*/
			assert(nextrec.fnext_[sym(next)&7] == sym((tid<<3)+i));
		}
		else
			assert(record.fnext_[(i+3)%6] == -1);
	}
	//adjlist is also reasonable.
	if(autoMerge)
	for(int i = 0;i<3;i++)
	{
		int tid = triPoints.size();
		int p1 = i%3;
		int p2 = (i+1)%3;
		TriEdge currEdge = TriEdge(record.vi_[p1], record.vi_[p2]);
		std::unordered_map<TriEdge, OrTri>::const_iterator triIter = triAdj.find(currEdge);
		if(triIter == triAdj.end())
			assert(false); //some record must exist
		else
		{
			OrTri reftri = triAdj[currEdge];
			assert((reftri>>3)<=triPoints.size());
			if((reftri>>3) == tid)
			{
				assert((reftri&7) == i || (reftri&7) == i+3);
			}
		}
	}
#endif
	triPoints.push_back(record);
	return (triPoints.size());
}



void Trist::delTri(OrTri ef){
	// Delete a triangle, but you can assume that this is ONLY used by the IP operation
	// You may want to make sure all its neighbours are detached (below)

	// This removes the tri from the triPoints table by swapping it with the last item,
	//  and then removing it (since that's easier).
	// Ultimately, doing in this way CHANGES TRIANGLE INDEXES.. which means we can't quite
	//  keep OrTri values outside of Trist.
	// An alternative might be to just "mark as deleted", and keep track of num_triangles as
	//  an additional variable.

	

	int tIdx = (ef >> 3);
#ifndef NDEBUG
	//fdetach(ef);
	/* assert that all my neighbours point back to me.

	*/
	for(int i = 0;i<3;i++)
	{
		TriRecord  myrecord = triPoints[tIdx]; 
		OrTri next = myrecord.fnext_[i];
		if(next!=-1)
		{
		OrTri prev = myrecord.fnext_[i+3];
		OrTri me = (tIdx<<3)+i;
		assert(fnext(sym(next)) == sym(me));
		assert(fnext(sym(prev)) == me);
		}
		else
			assert(myrecord.fnext_[i+3]==-1);
	}
#endif
	int tid = tIdx;
	int last = triPoints.size() - 1;
	TriRecord record = triPoints[tid];

	//get all references to me inside triadj and remove them
		//this is done before swapping so it's easier.
	for(int i = 0;i<3;i++)
	{
		int p1 = i%3;
		int p2 = (i+1)%3;
		TriEdge currEdge = TriEdge(record.vi_[p1], record.vi_[p2]);
		std::unordered_map<TriEdge, OrTri>::const_iterator triIter = triAdj.find(currEdge);
		if(triIter != triAdj.end())
		{
			OrTri Xtri = triAdj[currEdge];
			//determine if it's me.
			if(Xtri>>3 == tid)
			{
				//i either have a neighbour
				if(fnext(Xtri) > -1)
				{
					assert(fnext(sym(Xtri))>>3 == fnext(Xtri)>>3); //2d only.
					assert(fnext(sym(fnext(Xtri))) == sym(Xtri));
					assert(fnext(sym(fnext(sym(Xtri)))) == Xtri);
					triAdj[currEdge] = fnext(Xtri);
					
				}
				else //or i do not.
				{
					assert(fnext(sym(Xtri)) == -1);
					triAdj.erase(currEdge);
				}
			}
			else
			{
				assert(fnext(Xtri)>>3 == tid); 
				assert(fnext(sym(Xtri))>>3 == tid);
				//not me then i'm it's neighbour.
				//this is only for 2d case.
			}
		}
	}
#ifndef NDEBUG
	for (auto it = triAdj.begin(); //screw you, i'm too lazy to figure out type xD
        it != triAdj.end(); ++it) 
	{
		assert((it->second)>>3 != tid);
	}
#endif
	//swap positions if necessary
	if(tIdx < last){
		// If ef isn't the last triangle..
		relableRefsToTri(last, tIdx);  //this relables ALL references to last triangle correctly to tidx.
			//including the triadj references.
		triPoints[tIdx] = triPoints[last]; //unilateral swap.
		//relable all points in the record i have to point to correct version if need be.
		for(int i = 0;i<6;i++)
			if(record.fnext_[i]>>3 == last)
				record.fnext_[i] = ((tid<<3) + ((record.fnext_[i])&7));

		std::cout<<"relabling complete"<<std::endl;
	}

	//at this point, i know who my neighbours are.
		//remove myself from all neighbours.
	for(int i = 0;i<3;i++)
	{
		if(record.fnext_[i] != -1)
		{
			OrTri tnext = record.fnext_[i];
			TriRecord *prevrecord = &triPoints[tnext>>3];
			OrTri tprev = record.fnext_[i+3];
			assert(tprev!=-1);
			TriRecord *nextrecord = &triPoints[tprev>>3];
			if(sym(tprev) == tnext)
			{
				nextrecord->fnext_[tnext&7] = -1;
				nextrecord->fnext_[sym(tnext)&7] = -1;
			}
			else
			{
				assert(false); //3d code.
				prevrecord->fnext_[sym(tprev)&7] = tnext;
				nextrecord->fnext_[sym(tnext)&7] = tprev;
			}
		}
		else
		{
			assert(record.fnext_[i+3] == -1);
		}
	}

	/* //all reference to last, switch to tidx
	//remove triangle from all neighbours.
	for(int i = 0;i<3;i++)
	{
		
		int p1 = i%3;
		int p2 = (i+1)%3;
		int opposite = (i+2)%3;
		
		OrTri me = (tid<<3) + i;
		//tprev and tnext have the same orientation.
		OrTri tnext = fnext(me);
		assert(tnext>=-1);
		if(tnext>>3 == last && tnext!=-1)
			tnext = tIdx+(tnext&7);
		if(tnext != -1)
		{
			OrTri tprev = sym(fnext(sym(me))); 
			if(tprev>>3 == last)
				tprev = tIdx+(tprev&7);
			if(tnext == tprev) //they are the same
			{
				TriRecord nextrecord = triPoints[tnext>>3];
				nextrecord.fnext_[tnext&7] = nextrecord.fnext_[sym(tnext)&7] = -1;
				triPoints[tnext>>3] = nextrecord; //hack
				//need to figure out how to check for update.
			}
			else
			{
				assert(tprev!= 1); //by symmetry.
				assert(false);
				//this should not be possible now in 2d space.
				//but this should be correct.
				TriRecord nextrecord = triPoints[tnext>>3];
				TriRecord prevrecord = triPoints[tprev>>3];
				nextrecord.fnext_[sym(tnext)&7] = sym(tprev);
				prevrecord.fnext_[tprev] = tnext;
			}
		}
		else //tnext == -1
			assert(fnext(sym(me)) == -1); //symmetric
	}
	//check if i am referred to by triadj
	for(int i = 0;i<3;i++)
	{
		//int tid = tIdx;
		int p1 = i%3;
		int p2 = (i+1)%3;
		//int opposite = (i+2)%3;
		//TriRecord record = triPoints[tid];
		TriEdge currEdge = TriEdge(record.vi_[p1], record.vi_[p2]);
		std::unordered_map<TriEdge, OrTri>::const_iterator triIter = triAdj.find(currEdge);
		if(triIter != triAdj.end())
		{
			OrTri Xtri = triAdj[currEdge];
			//determine if it's me.
			if((Xtri>>3) == tid)
			{
				int orientation = Xtri&7; //do i have neighbours in that direction?
				//if i do, then i pick one and use it. (i don't care which orientation, it is figured out on adding)
				if(record.fnext_[orientation]!=-1)
				{
					assert((fnext(tid)>>3) == (fnext(sym(tid))>>3));
					triAdj[currEdge] = record.fnext_[orientation];
				}//else, i'm the only one
				else
				{
					assert(record.fnext_[sym(Xtri)&7]==-1);
					triAdj.erase(currEdge);
					std::cout<<"erased edge"<<std::endl;
				}
			}
			else //else don't care.
			{
				//for 2d case.
				assert((fnext(Xtri)>>3)==last); //i'm now at the end, so if i'm not the triangle being removed,
				//then it's not me it must be my neighbour (because my edge is being removed)
				//the reference should be updated by now.
			}
			
		}
		else
			assert(false); //i'm removing a triangle that's not supposed to exist
	}*/

	//remove references.

	/*for(int i = 0;i<3;i++)
	{
	// for each edge, check if is in the hash list
		int tid = tIdx;
		int p1 = i%3;
		int p2 = (i+1)%3;
		int opposite = (i+2)%3;
		TriRecord record = triPoints[tid];
		TriEdge currEdge = TriEdge(record.vi_[p1], record.vi_[p2]);
		OrTri me = (tid<<3) + i;
		OrTri tnext = fnext(me);
		OrTri tprev = sym(fnext(sym(me)));
		std::unordered_map<TriEdge, OrTri>::const_iterator triIter = triAdj.find(currEdge);
		if(triIter != triAdj.end())
		{
			OrTri Xtri = triAdj[currEdge];
			std::cout<<"Reference tid = "<<(Xtri&7)<<" my id = "<<tid<<std::endl;
			std::cout<<"Reference oid = "<<Xtri<<" my id = "<<me<<std::endl;
			if(Xtri == me) //that means the reference triangle is me.
			{//transfer the reference triangle to my neighbour if need be.
				if(record.fnext_[i] == -1) //i don't need to, there is no other triangle with that edge.
				{
					assert(record.fnext_[i+3] == -1); //by symmetry
					//remove from records.
					triAdj.erase(currEdge);
					std::cout<<"deleted from triadj"<<std::endl;
				}
				else
				{
					//then point to a neighbour.
					triAdj[currEdge] = tnext;
					std::cout<<"pointed to "<<tnext<<std::endl;
				}
			}
		}
		else
		{
			assert(false); //there are no triangles with this edge in triadj, but i have one.
		}
		if(record.fnext_[i] != -1) //exists some other triangle with that edge.
		{
			assert(record.fnext_[i+3] != -1);
			//connect both ways
			TriRecord tprevRec = triPoints[tprev>>3];
			TriRecord tnextRec = triPoints[tnext>>3];
			tprevRec.fnext_[tprev&7] = tnext;
			tnextRec.fnext_[sym(tnext&7)] = sym(tprev);
		}
	}*/
	triPoints.pop_back();
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

	return (OrTri) (tId << 3) + ((v + 3) % 6);
}



OrTri Trist::fnext(OrTri ef){
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.

	// Get the tIdth triangle record.
	TriRecord record = triPoints[tId];

	//return (OrTri) (tId << 3) + (record.fnext_[v]);
	return record.fnext_[v];
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
	return (v < 3) ? v : (v + 1) % 3;
}



int Trist::dest(OrTri ef){
	// the index of the second vertex of OrTri, e.g. org(bcd) => c
	int tId = ef >> 3;
	int v = ef & 7; // ef & 00111b ... last 3 bits.
	TriRecord record = triPoints[tId];

	// See formula in trist.h
	return (v < 3) ? (v + 1) % 3 : v;
}



void Trist::fmerge(OrTri abc, OrTri abd){
	assert(false); //untested, probably broken.
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



void Trist::relableRefsToTri(int oldtid, int newtid){
	// Slight misnomer; this renames 
	// ASSUME 2D ONLY
	// this assumption allows us to just not give a toss.
	
	TriRecord *r = &(triPoints[oldtid]);
	
	// Check all the triangles the tri oldEF is connected to,
	// and then set their references to oldEF to point to newEF.
	for(int v = 0; v < 6; v++){
		if(r->fnext_[v] >= 0){
			int nextTIdx = ((r->fnext_[v]) >> 3);
			int nextV = ((r->fnext_[v]) & 7);
			TriRecord *nextT = &(triPoints[nextTIdx]);

			//point back to myself, but sym.
			OrTri self = nextT->fnext_[(nextV+3)%6];
			assert(self>>3 == oldtid);
			nextT->fnext_[(nextV+3)%6] =  (newtid<<3)+(self&7); //orientation is preserved.
		}
	}
	//make sure all triadj pointers to me have been relabeled.
	for(int i=0;i<3;i++)
	{
		int p1 = i%3;
		int p2 = (i+1)%3;
		TriEdge e = TriEdge(r->vi_[p1], r->vi_[p2]);
		assert(triAdj.find(e) != triAdj.end());
		OrTri curr = triAdj[e];
		if(curr>>3 == oldtid)
			triAdj[e] = (newtid<<3) + (curr&7);

	}
}


void Trist::fdetach(OrTri abc){
	// detach triangle abc with all its neighbours (undo fmerge)
	relableRefsToTri(abc>>3, -1);

	TriRecord *r = &(triPoints[abc >> 3]);
	r->fnext_[0] = -1;
	r->fnext_[1] = -1;
	r->fnext_[2] = -1;
	r->fnext_[3] = -1;
	r->fnext_[4] = -1;
	r->fnext_[5] = -1;
}



void Trist::eraseAllTriangles(){
	triPoints.clear();
}



void Trist::incidentTriangles(int ptIndex, int& noOrTri, OrTri* otList){
	// A suggested function: you may want this function to return all the OrTri
	// that are incident to this point
	// Ignore this if you don't feel a need

	noOrTri = 0;

	for(unsigned int i = 0; i < triPoints.size(); i++){
		TriRecord *r = &(triPoints[i]);

		if(r->vi_[0] == ptIndex || r->vi_[1] == ptIndex || r->vi_[2] == ptIndex){
			   noOrTri = noOrTri + 1;
			   otList = (OrTri*) realloc(otList, noOrTri * sizeof(OrTri));

			   otList[noOrTri - 1] = i;
		}
	}
}
