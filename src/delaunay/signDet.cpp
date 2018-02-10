#include "delaunay/signDet.h"

#include <iostream>

#include "delaunay/longint/li.h"

using std::cout;
using std::endl;


namespace delaunay {

template<typename I>
I determinant(I x1, I y1, I w1,
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
	// cout << "[signDet(x1=" << x1 << ", y1=" << y1 << ", w1=" << w1 << "," << endl;
	// cout << "         x2=" << x2 << ", y2=" << y2 << ", w2=" << w2 << "," << endl;
	// cout << "         x3=" << x3 << ", y3=" << y3 << ", w3=" << w3 << ")]" << endl;

	I det = x1*(y2*w3 - y3*w2)
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


template int signDet<double>(double, double, double,
                             double, double, double,
                             double, double, double);

}

