#include "stdafx.h"
#include "directedGraph.h"
#include <iostream>


// Vector that keeps the order in which keys got inserted into DAG. Remove if not needed.
std::vector<TriRecord> orderedkeyList; 
std::vector<TriRecord> leafnodeList;


DirectedGraph::DirectedGraph(PointSetArray &pSet) {
	triVertices = &pSet;
}

std::vector<TriRecord> DirectedGraph::getLeafNodes() {
	return leafnodeList;
}

// This method adds children nodes to the specified parent node. This is only for new point additions and not for flipping.
void DirectedGraph::addChildrenNodes(int pIdX){	
	
	std::cout << "inside addchildrennodes:" << pIdX << std::endl;
	
	// Find triangle to which the point belongs. This function returns the bounding triangle in the case that the 
	// point passed here is the end vertex of the bounding triangle
	TriRecord containingTriangle = findLeafNodeForPoint(pIdX);

	
	// Declare iterator for the dagNode, and the vector of TriRecords for the children of the node.
	std::map<TriRecord,std::vector<TriRecord> >::iterator iter;
	std::vector<TriRecord> existingChildren;
	TriRecord child1, child2, child3;
	int pIndex1, pIndex2, pIndex3;

	//iter = dagNode.find(containingTriangle);

	// Get the vertices of the parent triangle 
	pIndex1 = containingTriangle.vi_[0];
	pIndex2 = containingTriangle.vi_[1];
	pIndex3 = containingTriangle.vi_[2];

	// Construct 3 Trirecords, one for each child triangle and add it to existingChildren.	
	child1.vi_[0] = pIndex1;
	child1.vi_[1] = pIndex2;
	child1.vi_[2] = pIdX;
	child2.vi_[0] = pIndex2;
	child2.vi_[1] = pIndex3;
	child2.vi_[2] = pIdX;
	child3.vi_[0] = pIndex3;
	child3.vi_[1] = pIndex1;
	child3.vi_[2] = pIdX;
	existingChildren.push_back(child1);
	existingChildren.push_back(child2);
	existingChildren.push_back(child3);

	// Insert the parent-child relationship into the DAG, only if the containing triangle itself is not the bounding triangle.
	if(pIdX != pIndex1 && pIdX != pIndex2 && pIdX != pIndex3)
	{
		dagNode.insert(std::map<TriRecord, std::vector<TriRecord>>::value_type(containingTriangle, existingChildren));
		leafnodeList.push_back(child1);
		leafnodeList.push_back(child2);
		leafnodeList.push_back(child3);
	}
	else leafnodeList.push_back(containingTriangle);
	// Push the parent node into the orderedkeylist. This will preserve the order in which parents have been inserted.
	// First record will always be the bounding triangle, which we will use in the findLeafNodeforPoint for setting the
	// initial worklist
	

	orderedkeyList.push_back(containingTriangle);
	
	// Section for removing the newly created parent node from the leafnodeList.
	std::vector<TriRecord>::iterator it;
	for(it = leafnodeList.begin(); it != leafnodeList.end();)
	{
		TriRecord tri = *it;
		if(tri.vi_[0]==containingTriangle.vi_[0] && tri.vi_[1]==containingTriangle.vi_[1] 
		&& tri.vi_[2]==containingTriangle.vi_[2])
		{
			it = leafnodeList.erase(it);
		}
		else
		{
			++it;
		}
	}	

	
}

// This method returns the triangle which encloses the input point.
TriRecord DirectedGraph::findLeafNodeForPoint(int pIdX){
	
	std::vector<TriRecord> worklist;
	std::vector<TriRecord> ::iterator iter;
	int pIndex1,pIndex2, pIndex3;
	

	TriRecord rootNode;

	// If the root node has no edges, it is the bounding triangle. 
	if(!orderedkeyList.empty())
	{
		rootNode = orderedkeyList.front();
		
		if(dagNode.size() == 0) // Happens when the ordered list only has bounding triangle
		{
			worklist.push_back(rootNode);
		}

		else
		{
			worklist = dagNode.find(rootNode) ->second;
		}
	}
	else
	{
		// Return the trirecord composed of last 3 entries from myPoints pointset. The incoming point ID is the last vertex of 
		// the bounding triangle.
		TriRecord boundingTri;
		boundingTri.vi_[0] = pIdX;
		boundingTri.vi_[1] = pIdX - 1;
		boundingTri.vi_[2] = pIdX - 2;
		return boundingTri;
		
	}
	
	// Iterate through the worklist and find the containing triangle. Once found, replace the worklist with 
	// the new worklist of this triangle and continue iteration.
	

	for(iter=worklist.begin(); iter != worklist.end(); )
	{		
		TriRecord checkTriangle = *iter;
		pIndex1 = checkTriangle.vi_[0];
		pIndex2 = checkTriangle.vi_[1];
		pIndex3 = checkTriangle.vi_[2];

		
		int ret = (*triVertices).inTri(pIndex1,pIndex2,pIndex3,pIdX); // If ret is >=0, the point is inside the triangle
		if(ret >=0)
		{
			
			if( dagNode.find(checkTriangle) != dagNode.end())
			{
				worklist = dagNode.find(checkTriangle) ->second; // There could be error here. Does C++ alow us to change the object we are iterating through?
				iter = worklist.begin();
			}
			else
			{
				return checkTriangle;
			}
		}
		else 
		{
			++iter;
		}
	
	}

}

// This method returns the set of 2 triangles the input edge belongs to.
std::vector<TriRecord> DirectedGraph::findNodesForEdge(int pIdx1, int pIdx2 )
{
	
	std::vector<TriRecord> ::iterator iter;
	std::vector<TriRecord> outputlist;
	int pIndex1,pIndex2, pIndex3;


	for(iter=leafnodeList.begin(); iter != leafnodeList.end(); )
	{		
		TriRecord checkTriangle = *iter;
		pIndex1 = checkTriangle.vi_[0];
		pIndex2 = checkTriangle.vi_[1];
		pIndex3 = checkTriangle.vi_[2];

		if(pIndex1 == pIdx1 || pIndex2 == pIdx1 || pIndex3 == pIdx1)
		{
			if(pIndex1 == pIdx2 || pIndex2 == pIdx2 || pIndex3 == pIdx2)
			{
				outputlist.push_back(checkTriangle);
			}
		}

		++iter;

	}
	return outputlist;

}

// Please note that the edge to be flipped here is the 2nd and 3rd parameters.
void DirectedGraph::addFlipChildrenNodes(int pIdx1, int pIdx2, int pIdx3, int pIdx4)
{
	
	std::vector<TriRecord> parentTriangles;
	std::vector<TriRecord>::iterator iter;
	parentTriangles = findNodesForEdge(pIdx2, pIdx3);

	TriRecord newTri1, newTri2;
	newTri1.vi_[0] = pIdx1;
	newTri1.vi_[1] = pIdx2;
	newTri1.vi_[2] = pIdx4;

	newTri2.vi_[0] = pIdx1;
	newTri2.vi_[1] = pIdx3;
	newTri2.vi_[2] = pIdx4;

	std::vector<TriRecord> children;
	children.push_back(newTri1);
	children.push_back(newTri2);

	//Add the new triangles into the leafnodeList
	leafnodeList.push_back(newTri1);
	leafnodeList.push_back(newTri2);

	for(iter = parentTriangles.begin(); iter != parentTriangles.end(); )
	{
		TriRecord triangle = *iter;
		dagNode.insert(std::map<TriRecord, std::vector<TriRecord>>::value_type(triangle, children));
		++iter;

		//Remove the new parents from leafnodeList
		std::vector<TriRecord>::iterator it;
		for(it = leafnodeList.begin(); it != leafnodeList.end();)
		{
			TriRecord tri = *it;
			if(tri.vi_[0]==triangle.vi_[0] && tri.vi_[1]==triangle.vi_[1] 
			&& tri.vi_[2]==triangle.vi_[2])
			{
				it = leafnodeList.erase(it);
			}
			else
			{
				++it;
			}
		}	

	}	
}

// Removes everything from DAG
void DirectedGraph::cleardirectedGraph(){
	dagNode.clear();
	leafnodeList.clear();
	orderedkeyList.clear();
	int temp =1;

}