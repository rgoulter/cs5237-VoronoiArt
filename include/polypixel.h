#ifndef CS5237_POLYPIXELH
#define CS5237_POLYPIXELH

#include "pointSetArray.h"
#include "platform.h"
#include "tinydir.h"
#include <vector>
#include <string>

class ColoredPolygon {
public:
	std::vector<MyPoint> poly;
	float rgb[3];
};



// std::vector<int> enumeratePixelsInPolygon(const std::vector<int>& poly);

void findAverageColor3iv(GLuint glTex, const std::vector<MyPoint>& poly, int* colorIv);


bool findShaderDirectory(std::string& path, std::string shaderFilename);

#endif