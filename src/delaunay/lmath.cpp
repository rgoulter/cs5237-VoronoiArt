#include "lmath.h"

#include <iostream>



LongInt determinant(LongInt x1, LongInt y1, LongInt w1,
                    LongInt x2, LongInt y2, LongInt w2,
                    LongInt x3, LongInt y3, LongInt w3) {
	return x1*(y2*w3 - y3*w2)
	     - x2*(y1*w3 - y3*w1)
	     + x3*(y1*w2 - y2*w1);
}



int signDet(LongInt x1, LongInt y1, LongInt w1,
            LongInt x2, LongInt y2, LongInt w2,
            LongInt x3, LongInt y3, LongInt w3) {
	LongInt det = x1*(y2*w3 - y3*w2)
	            - x2*(y1*w3 - y3*w1)
	            + x3*(y1*w2 - y2*w1);

	if (det > 0)
		return 1;
	if (det < 0)
		return -1;
	return 0;
}

