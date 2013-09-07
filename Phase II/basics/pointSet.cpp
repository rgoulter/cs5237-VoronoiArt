#include "pointSet.h"
#include <iostream>
int orientation(MyPoint p1, MyPoint p2, MyPoint p3);
bool intersects(MyPoint a, MyPoint b, MyPoint c, MyPoint d);

int PointSet::addPoint(LongInt x1, LongInt y1){
	MyPoint thisPoint;
	thisPoint.x = x1;
	thisPoint.y = y1;
	thisPoint.z = 0;

	myPoints.push_back(thisPoint);
	return (myPoints.size());
}



int PointSet::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) {
	MyPoint p1 = myPoints[p1Idx];
	MyPoint p2 = myPoints[p2Idx];
	MyPoint p3 = myPoints[p3Idx];
	MyPoint v = myPoints[pIdx]; // the other point.
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



//this function gives an incorrect response for 1-2 cases per trial.
	//The only possiblity is my orientation function or whatever it depends on has a bug.
	//mainly because this code is stupidly simple.
int PointSet::inTri(int p1Idx, int p2Idx, int p3Idx, int pIdx) {
	//If v is in the triangle formed by (p1, p2), (p2, p3), (p3, p1)
	//then v is consistently on the same side of all of those lines.

	MyPoint p1 = myPoints[p1Idx];
	MyPoint p2 = myPoints[p2Idx];
	MyPoint p3 = myPoints[p3Idx];
	MyPoint v = myPoints[pIdx]; // the other point.

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

int orientation(MyPoint p1, MyPoint p2, MyPoint p3)
{
	return signDet(p1.x, p1.y, LongInt(1),
				   p2.x, p2.y, LongInt(1),
				   p3.x, p3.y, LongInt(1));

}
