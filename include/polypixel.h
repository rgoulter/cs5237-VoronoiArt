#ifndef CS5237_POLYPIXELH
#define CS5237_POLYPIXELH

#include <string>
#include <vector>

#include "delaunay/pointsetarray.h"

#include "imagedata.h"
#include "platform.h"



class ColoredPolygon {
public:
	std::vector<int> poly;
	float rgb[3];
};



void findSomeColor3iv(const ImageData& imData, const std::vector<int>& poly, int* colorIv);
void findSomeColor3iv(const ImageData& imData, delaunay::PointSetArray& psa, int* colorIv);


void findAverageColor3iv(const ImageData& imData, const std::vector<int>& poly, int* colorIv);
void findAverageColor3iv(const ImageData& imData, const std::vector<delaunay::MyPoint>& poly, int* colorIv);


bool findShaderDirectory(std::string& path, std::string shaderFilename);

#endif
