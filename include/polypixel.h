#ifndef CS5237_POLYPIXELH
#define CS5237_POLYPIXELH

#include <string>
#include <vector>

#include "geometry/polygon.h"

#include "imagedata.h"
#include "platform.h"



class ColoredPolygon {
public:
	geometry::Polygon poly;
	float rgb[3];
};


std::vector<ColoredPolygon> generateColoredPolygons(const std::vector<geometry::Polygon>& polys, const ImageData& imData);


void findSomeColor3iv(const ImageData& imData, const geometry::Polygon& poly, int* colorIv);


void findAverageColor3iv(const ImageData& imData, const geometry::Polygon& poly, int* colorIv);


bool findShaderDirectory(std::string& path, std::string shaderFilename);

#endif
