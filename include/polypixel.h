#ifndef CS5237_POLYPIXELH
#define CS5237_POLYPIXELH

#include "pointSetArray.h"
#include "platform.h"
#include <vector>
#include <string>

class ColoredPolygon {
public:
	std::vector<int> poly;
	float rgb[3];
};


std::vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa);


// std::vector<int> enumeratePixelsInPolygon(const std::vector<int>& poly);

void findSomeColor3iv(const std::vector<int>& poly, int* colorIv);
void findSomeColor3iv(PointSetArray& psa, int* colorIv);


void findAverageColor3iv(const std::vector<int>& poly, int* colorIv);
void findAverageColor3iv(const std::vector<MyPoint>& poly, int* colorIv);


bool findShaderDirectory(std::string& path, std::string shaderFilename);

#endif