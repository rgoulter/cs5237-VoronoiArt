#include "lmath.h"
#include <iostream>

LongInt determinant(LongInt x1, LongInt y1, LongInt w1,
			LongInt x2, LongInt y2, LongInt w2,
			LongInt x3, LongInt y3, LongInt w3);


int signDet(LongInt x1, LongInt y1, LongInt w1,
			LongInt x2, LongInt y2, LongInt w2,
			LongInt x3, LongInt y3, LongInt w3){

	//LongInt det = x1*(y2*w3-y3*w2)-y1*(x2*w3-x3*w2)+w1*(x2*y3-x3*y2);
	LongInt det = x1*(y2*w3-y3*w2) - x2*(y1*w3-y3*w1) + x3*(y1*w2-y2*w1); //my attempt.
	//LongInt det = x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2); //this computes the z value only
	//LongInt det2 = x1*(y2*w3-y3*w2)-y1*(x2*w3-x3*w2)+w1*(x2*y3-x3*y2);
	//if(det2*det<0)
		//std::cout<<"hmmm what is going on"<<std::endl;
	if(det > 0) return 1;
	if(det < 0) return -1;
	return 0;
}

int signDet(LongInt x[3][3]){
	return signDet(x[0][0], x[1][0], x[2][0],
				   x[0][1], x[1][1], x[2][0],
				   x[0][2], x[1][2], x[2][2]);
	//this is easier to maintain, since i'm going to hack it after this.
	/*LongInt det = x[0][0]*(x[1][1]*x[2][2]-x[2][1]*x[1][2])-
				  x[0][1]*(x[1][0]*x[2][2]-x[2][0]*x[1][2])+
				  x[0][2]*(x[1][0]*x[2][1]-x[2][0]*x[1][1]);

	if(det > 0) return 1;
	if(det < 0) return -1;
	return 0;*/
}

int signDet(LongInt x1, LongInt y1, LongInt w1, LongInt z1,
			LongInt x2, LongInt y2, LongInt w2, LongInt z2,
			LongInt x3, LongInt y3, LongInt w3, LongInt z3,
			LongInt x4, LongInt y4, LongInt w4, LongInt z4){

	LongInt Det1, Det2, Det3, Det4;
	Det1 = determinant(y2, w2, z2,
					   y3, w3, z3,
					   y4, w4, z4);
	Det2 = determinant(y1, w1, z1,
					   y3, w3, z3,
					   y4, w4, z4);
	Det2 = determinant(y1, w1, z1,
					   y2, w2, z2,
					   y4, w4, z4);
	Det2 = determinant(y1, w1, z1,
					   y2, w2, z2,
					   y3, w3, z3);
	LongInt det = x1*Det1 - y1*Det2 + w1*Det3 - z1*Det4;
	
	if(det > 0) return 1;
	if(det < 0) return -1;
	return 0;
}

LongInt determinant(LongInt x1, LongInt y1, LongInt w1,
			LongInt x2, LongInt y2, LongInt w2,
			LongInt x3, LongInt y3, LongInt w3){
		return x1*(y2*w3-y3*w2) - x2*(y1*w3-y3*w1) + x3*(y1*w2-y2*w1);
}