#ifndef CS5237_GENPROBDISTH
#define CS5237_GENPROBDISTH

#include <string>
#include <utility>
#include <vector>

#include "opencv2/imgproc/imgproc.hpp"

#include "imagedata.h"
#include "platform.h"



// generatePointsWithPDF loads values into textures,
// using `generateOGLTextureForOpenCVMat`.
// Use this struct to keep track of these
//
// Since ImageData keeps cv::Mat, and cv::Mat is cheap to copy,
// should be ok to do like this.
struct PDFTextures {
	ImageData* edgesTexture = nullptr;
	ImageData* edgesSharpTexture = nullptr;
	ImageData* edgesBlurTexture = nullptr;
	ImageData* pdfTexture = nullptr;
};



std::vector<std::pair<int, int>> generateUniformRandomPoints(int width, int height, int numPoints);

PDFTextures pdfTexturesFromImage(const cv::Mat& imgMat, int cannyRatio = 3, int kernelSize = 3);

const cv::Mat& distributionFromPDFTextures(PDFTextures pdfTextures);

std::vector<std::pair<int, int>> generatePointsFromDistributionField(const cv::Mat& distribution, int numPoints);

#endif
