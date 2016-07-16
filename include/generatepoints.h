#ifndef CS5237_GENPROBDISTH
#define CS5237_GENPROBDISTH

#include <string>
#include <utility>
#include <vector>

#include "imagedata.h"
#include "platform.h"



// generatePointsWithPDF loads values into textures,
// using `generateOGLTextureForOpenCVMat`.
// Use this struct to keep track of these
//
// Since ImageData keeps cv::Mat, and cv::Mat is cheap to copy,
// should be ok to do like this.
struct PDFTextures {
	ImageData *edgesTexture;
	ImageData *edgesSharpTexture;
	ImageData *edgesBlurTexture;
	ImageData *pdfTexture;
};



std::vector< std::pair<int,int> > generateUniformRandomPoints(int width, int height, int numPoints);
std::vector< std::pair<int,int> > generatePointsWithPDF(std::string filename, int numPoints, PDFTextures* tex, int cannyRatio = 3, int kernelSize = 3);

#endif
