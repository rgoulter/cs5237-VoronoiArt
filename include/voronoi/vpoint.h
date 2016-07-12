#ifndef VORONOI_VPoint_h
#define VORONOI_VPoint_h



namespace voronoi {

/*
    A structure that stores 2D point
*/

struct VPoint {
public:
	double x, y;

	/*
	    Constructor for structure; x, y - coordinates
	*/

	VPoint(double nx, double ny) {
		x = nx;
		y = ny;
	}
};

}

#endif

