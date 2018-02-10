#ifndef DELAUNAY_SIGNDETH
#define DELAUNAY_SIGNDETH



namespace delaunay {

/// return 1 as positive, -1 as negative, 0 as zero of the determinant
template<typename I>
int signDet(I x1, I y1, I w1,
            I x2, I y2, I w2,
            I x3, I y3, I w3);

}

#endif
