#include "delaunay/signDet.h"

#include <iostream>

#include "delaunay/longint/li.h"



namespace delaunay {

template<typename I>
LongInt determinant(I x1, I y1, I w1,
                    I x2, I y2, I w2,
                    I x3, I y3, I w3) {
	return x1*(y2*w3 - y3*w2)
	     - x2*(y1*w3 - y3*w1)
	     + x3*(y1*w2 - y2*w1);
}



template<typename I>
int signDet(I x1, I y1, I w1,
            I x2, I y2, I w2,
            I x3, I y3, I w3) {
	LongInt det = x1*(y2*w3 - y3*w2)
	            - x2*(y1*w3 - y3*w1)
	            + x3*(y1*w2 - y2*w1);

	if (det > 0)
		return 1;
	if (det < 0)
		return -1;
	return 0;
}



// Instantiate function
template int signDet<LongInt>(LongInt, LongInt, LongInt,
                              LongInt, LongInt, LongInt,
                              LongInt, LongInt, LongInt);

template int signDet<int>(int, int, int,
                          int, int, int,
                          int, int, int);

}

