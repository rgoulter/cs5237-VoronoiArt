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
	LongInt magnitudev_sq = v.x*v.x + v.y*v.y;
	return signDet(p1.x-v.x, p1.y-v.y, p1.x*p1.x+p1.y*p1.y - magnitudev_sq,
				   p2.x-v.x, p2.y-v.y, p2.x*p2.x+p2.y*p2.y - magnitudev_sq,
				   p3.x-v.x, p3.y-v.y, p1.x*p3.x+p3.y*p3.y - magnitudev_sq)>=0;
}
/*int PointSet::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) {
	MyPoint p1 = myPoints[p1Idx];
	MyPoint p2 = myPoints[p2Idx];
	MyPoint p3 = myPoints[p3Idx];
	MyPoint v = myPoints[pIdx]; // the other point.
	//if any 2 points are the same, return 0 immediately.
	if(p1 == p2 || p2 == p3 || p1 == p3)
		return -2000;
	int orient = orientation(p1, p2, p3);
	if(orient == 0) //if they are colinear.
		return -1000;
	//look here for formula: http://www.cs.cmu.edu/~quake/robust.html
	LongInt t1x, t1y, t2x, t2y, t3x, t3y;
	t1x = p1.x-v.x;
	t1y = p1.y-v.y;
	t2x = p2.x-v.x;
	t2y = p2.y-v.y;
	t3x = p3.x-v.x;
	t3y = p3.y-v.y;

	//return signDet(t1x, t1y, t1x*t1x + t1y*t1y,
	//				t2x, t2y, t2x*t2x + t2y*t2y,
	//				t3x, t3y, t3x*t3x + t3y*t3y)*orient>=0;
	return signDet(t1x, t1y, t1x*t1x + t1y*t1y,
					t2x, t2y, t2x*t2x + t2y*t2y,
					t3x, t3y, t3x*t3x + t3y*t3y)<=0; //quite a good solution, but not theoretically correct.
	//return signDet(t1x, t1y, t1x*(p1.x+v.x) + t1y*(p1.y+v.y),
	//				t2x, t2y, t2x*(p2.x+v.x) + t2y*(p2.y+v.y),
	//				t3x, t3y, t3x*(p3.x+v.x) + t3y*(p3.y+v.y)) == 0;
//bloody attempt n+1. Use lecturer's formula.
	//this is the theoretically correct solution.
	//return signDet(p1.x, p1.y, p1.x*p1.x+p1.y*p1.y, LongInt(1),
	//				p2.x, p2.y, p2.x*p2.x+p2.y*p2.y, LongInt(1),
	//				p3.x, p3.y, p3.x*p3.x+p3.y*p3.y, LongInt(1),
	//				v.x, v.y, v.x*v.x+v.y*v.y, LongInt(1))*orient;
}*/
/*
int PointSet::inCircle(int p1Idx, int p2Idx, int p3Idx, int pIdx) {

	int det1 = signDet(myPoints[p1Idx].x-myPoints[pIdx].x,myPoints[p1Idx].y-myPoints[pIdx].y,(myPoints[p1Idx].x-myPoints[pIdx].x)*(myPoints[p1Idx].x-myPoints[pIdx].x)+(myPoints[p1Idx].y-myPoints[pIdx].y)*(myPoints[p1Idx].y-myPoints[pIdx].y),
						   myPoints[p2Idx].x-myPoints[pIdx].x,myPoints[p2Idx].y-myPoints[pIdx].y,(myPoints[p2Idx].x-myPoints[pIdx].x)*(myPoints[p2Idx].x-myPoints[pIdx].x)+(myPoints[p2Idx].y-myPoints[pIdx].y)*(myPoints[p2Idx].y-myPoints[pIdx].y),
						   myPoints[p3Idx].x-myPoints[pIdx].x,myPoints[p3Idx].y-myPoints[pIdx].y,(myPoints[p3Idx].x-myPoints[pIdx].x)*(myPoints[p3Idx].x-myPoints[pIdx].x)+(myPoints[p3Idx].y-myPoints[pIdx].y)*(myPoints[p3Idx].y-myPoints[pIdx].y));
	int det2 = signDet(myPoints[p1Idx].x,myPoints[p1Idx].y,1,
					   myPoints[p2Idx].x,myPoints[p2Idx].y,1,
					   myPoints[p3Idx].x,myPoints[p3Idx].y,1);
	if(det1*det2>=0)
		return 1;
	else
		return 0;
	//if(det1==0 || det2==0) return 1;
	//if((det1<0 && det2<0) || (det1>0 && det2>0)) return 1;
	//return 0;
}*/


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
	/*return signDet(p1.x, p2.x, p3.x,
		           p1.y, p2.y, p3.y,
				   1, 1, 1);*/
	//should be no difference.
	return signDet(p1.x, p1.y, LongInt(1),
				   p2.x, p2.y, LongInt(1),
				   p3.x, p3.y, LongInt(1));

}

/*
int signArea(MyPoint p1, MyPoint p2, MyPoint p3)
{
	return orientation(p1, p2, p3);
}

bool intersects(MyPoint a, MyPoint b, MyPoint c, MyPoint d)
{
		return ((signArea(a,b,d)*signArea(a,b,c))<=0) && ((signArea(c,d,a)*signArea(c,d,b))<=0);
}*/