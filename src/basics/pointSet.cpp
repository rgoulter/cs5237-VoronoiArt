#include "pointSet.h"
#include <iostream>

int PointSet::addPoint(LongInt x1, LongInt y1){
	MyPoint thisPoint(x1, y1);

	myPoints.push_back(thisPoint);
	return (myPoints.size());
}

void PointSet::deleteLastPoint(){
	myPoints.erase(myPoints.begin()+myPoints.size()-1);
}


int PointSet::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) {
	// Indices given are 1-based, but stored as 0-based.
	MyPoint p1 = myPoints[p1Idx - 1];
	MyPoint p2 = myPoints[p2Idx - 1];
	MyPoint p3 = myPoints[p3Idx - 1];
	MyPoint v = myPoints[pIdx - 1]; // the other point.
	//if any 2 points are the same, return 0 immediately.
	if(p1 == p2 || p2 == p3 || p1 == p3)
		return 0;
	int orient = orientation(p1, p2, p3);
	if(orient == 0) //if they are colinear.
		return 1;
	//look here for formula: http://www.cs.cmu.edu/~quake/robust.html
	LongInt t1x, t1y, t2x, t2y, t3x, t3y;
	t1x = p1.x-v.x;
	t1y = p1.y-v.y;
	t2x = p2.x-v.x;
	t2y = p2.y-v.y;
	t3x = p3.x-v.x;
	t3y = p3.y-v.y;

	//this is a refractoring of the lecturer's solution to be a 3x3 determinant instead.
	return signDet(t1x, t1y, t1x*(p1.x+v.x) + t1y*(p1.y+v.y),
					t2x, t2y, t2x*(p2.x+v.x) + t2y*(p2.y+v.y),
					t3x, t3y, t3x*(p3.x+v.x) + t3y*(p3.y+v.y))*orient>=0;
}

// This method returns the circumcircle center for the three input points.
bool PointSet::circumCircle(int p1Idx, int p2Idx, int p3Idx, MyPoint& center){
    LongInt dA, dB, dC, aux1, aux2, div;
	int quo1, quo2, aux1Sign, aux2Sign;
	LongInt two = 2;
	MyPoint p1 = myPoints[p1Idx - 1];
	MyPoint p2 = myPoints[p2Idx - 1];
	MyPoint p3 = myPoints[p3Idx - 1];
	//p1.x = 0;p1.y=0,p2.x=0,p2.y=2;p3.x=1,p3.y=1;
    
	dA = p1.x * p1.x + p1.y * p1.y;
    dB = p2.x * p2.x + p2.y * p2.y;
    dC = p3.x * p3.x + p3.y * p3.y;
 
    aux1 = (dA*(p3.y - p2.y) + dB*(p1.y - p3.y) + dC*(p2.y - p1.y));	
    aux2 = -(dA*(p3.x - p2.x) + dB*(p1.x - p3.x) + dC*(p2.x - p1.x));	
    div = (two*(p1.x*(p3.y - p2.y) + p2.x*(p1.y-p3.y) + p3.x*(p2.y - p1.y)));
	
 
    if(div == 0){ 
        return false;
    }

	LongInt aux1sign = aux1.sign();
	LongInt aux2sign = aux2.sign();
	LongInt divsign = div.sign();
	LongInt aux1modval = aux1*aux1sign;
	LongInt aux2modval = aux2*aux2sign;
	LongInt divmodval = div*divsign;

	// Repeated subtraction for division
	for ( quo1 = 0; aux1modval - divmodval >= 0; quo1++ )
	{
		aux1modval = aux1modval - divmodval;
	}
	for ( quo2 = 0; aux2modval - divmodval >= 0; quo2++ )
	{
		aux2modval = aux2modval - divmodval;
	}
    
	center.x = ((LongInt)quo1)*aux1sign*divsign;
	center.y = ((LongInt)quo2)*aux2sign*divsign;
 
    return true;
}


//this function gives an incorrect response for 1-2 cases per trial.
//The only possiblity is my orientation function or whatever it depends on has a bug.
//mainly because this code is stupidly simple.
int PointSet::inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx) {
	//If v is in the triangle formed by (p1, p2), (p2, p3), (p3, p1)
	//then v is consistently on the same side of all of those lines.

	// Indices given are 1-based, but stored as 0-based.
	MyPoint p1 = myPoints[p1Idx - 1];
	MyPoint p2 = myPoints[p2Idx - 1];
	MyPoint p3 = myPoints[p3Idx - 1];
	MyPoint v = myPoints[pIdx - 1]; // the other point.

	//remove cases where any 2 points are equal.
	if(p1 == p2 || p2 == p3 || p3 == p1)
		return 0;
	int orientation1, orientation2, orientation3;
	//p1->p2
	orientation1 = orientation(p1, p2, v);
	//p2->p3
	orientation2 = orientation(p2, p3, v);
	//p3->p1
	orientation3 = orientation(p3, p1, v);
	//std::cout<<orientation1<<", "<<orientation2<<","<<orientation3<<std::endl;
	//if v is on any of the lines, we return 0.
	if(orientation1 == 0 || orientation2 == 0 || orientation3 == 0)
		return 0;
	else if(orientation1 == orientation2 && orientation2 == orientation3)
		return 1;
	else
		return -1;
}

