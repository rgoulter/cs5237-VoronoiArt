#ifndef CS5237_GENPROBDISTH
#define CS5237_GENPROBDISTH

#include <string>
#include <vector>

#include "platform.h"



// generatePointsWithPDF loads values into textures,
// using `generateOGLTextureForOpenCVMat`.
// Use this struct to keep track of these
struct PDFTextures {
	GLuint edgesTexture;
	GLuint edgesSharpTexture;
	GLuint edgesBlurTexture;
	GLuint pdfTexture;
};



std::vector<int> generateUniformRandomPoints(int width, int height, int numPoints);
std::vector<int> generatePointsWithPDF(std::string filename, int numPoints, PDFTextures* tex);

#endif
