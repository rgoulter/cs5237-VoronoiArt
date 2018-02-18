#include "generatepoints.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <set>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "imagedata.h"
#include "platform.h"

using std::default_random_engine;
using std::uniform_int_distribution;

using std::pair;
using std::set;
using std::string;
using std::vector;
using std::make_pair;

using std::min;

using cv::COLOR_BGR2GRAY;
using cv::COLOR_RGB2GRAY;
using cv::Mat;
using cv::Size;
using cv::Scalar;
using cv::imread;



// It would be nice to abstract these details and have some kind of "constant" PDF / mat,
// and generate points from that. Oh well.
//
// POINTREP:INTPAIRVEC
vector<pair<int,int>> generateUniformRandomPoints(int width, int height, int numPoints) {
	set<int> generated;

	// shouldn't call generateUniformRandomPoints with too many numPoints,
	// but since we want to generate unique, there's an upper limit.
	numPoints = min(numPoints, width * height - 1);


	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	uniform_int_distribution<int> distribution(0, width * height - 1);

	while (generated.size() < (unsigned) numPoints) {
		int coord = distribution(generator);

		generated.insert(coord);
	}

	vector< pair<int,int> > outputPts;

	for (int coord : generated) {
		int x = coord % width;
		int y = coord / width;
		outputPts.push_back(make_pair(x, y));
	}

	return outputPts;
}



PDFTextures pdfTexturesFromImage(const Mat& imgMat, int cannyRatio, int kernelSize) {
	if (!imgMat.data) {
		PDFTextures tmp;
		return tmp;
	}

	// Create a matrix of the same type and size as imgMat (for dst)
	Mat dst(imgMat.size(), imgMat.type());

	// Convert the image to grayscale
	Mat imgMat_gray;
	cvtColor(imgMat, imgMat_gray, COLOR_BGR2GRAY);

	// Reduce noise with a kernel 3x3
	Mat detected_edges;
	blur(imgMat_gray, detected_edges, Size(3,3));

	// Detect edges with Canny edge detector
	// void Canny(InputArray image, OutputArray edges, double threshold1, double threshold2, int apertureSize=3, bool L2gradient=false )
	// Reference: https://docs.opencv.org/2.4/modules/imgproc/doc/feature_detection.html#canny
	Canny(detected_edges, detected_edges, 100, 100 * cannyRatio, kernelSize);

	// This is to get P_sharp?
	Mat detected_edges2;
	// void GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX, double sigmaY=0)
	// Reference: https://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html#gaussianblur
	GaussianBlur(detected_edges, detected_edges2, Size(7, 7), 0, 0);

	dst = Scalar::all(0);
	Mat dst2;
	dst2 = Scalar::all(0);
	Mat dst3;
	dst3 = Scalar::all(0);

	// void Mat::copyTo(OutputArray m) const
	// void Mat::copyTo(OutputArray m, InputArray mask) const
	// Reference: https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat-copyto
	imgMat_gray.copyTo(dst2, detected_edges2);
	imgMat_gray.copyTo(dst, detected_edges);

	// This is to get P_blur?
	GaussianBlur(dst2, dst3, Size(15, 15), 0, 0);
	dst3.convertTo(dst3, -1, 2, 0);

	// Ps = Pblur - Psharp
	// void subtract(InputArray src1, InputArray src2, OutputArray dst)
	// Reference: https://docs.opencv.org/2.4/modules/core/doc/operations_on_arrays.html#subtract
	subtract(dst3, dst2, dst);

	// Make OpenGL Textures for the following:
	PDFTextures effectTextures;
	effectTextures.edgesTexture = new ImageData(detected_edges, CV_GRAY2RGB);
	effectTextures.edgesSharpTexture = new ImageData(detected_edges2, CV_GRAY2RGB);
	effectTextures.edgesBlurTexture = new ImageData(dst3, CV_GRAY2RGB);
	effectTextures.pdfTexture = new ImageData(dst, CV_GRAY2RGB);

	return effectTextures;
}



const Mat& distributionFromPDFTextures(PDFTextures pdfTextures) {
	ImageData* pdfTexture = pdfTextures.pdfTexture;
	return pdfTexture->getImageMat();
}



/// SMELL: dstRGB expected to be RGB encoded
vector<pair<int, int>> generatePointsFromDistributionField(const Mat& dstRGB, int numPDFPoints) {
	// So, let's build up a 1-D array of the CDF from the 2D PDF.
	//  idx -> (row, col) : row = idx / width, col = idx % width
	//  (row, col) -> idx : idx = row * width + col

	Mat dst;
	cvtColor(dstRGB, dst, COLOR_RGB2GRAY);

	// use float or uchar?
	vector<float> cdf; //loadedImageWidth * loadedImageHeight
	float sum = 0;

	for (int r = 0; r < dst.rows; r++) {
		for (int c = 0; c < dst.cols; c++) {
			unsigned char val = dst.at<unsigned char>(r, c);

			cdf.push_back(sum);
			sum += (float) val / 255;
		}
	}

	cdf.push_back(sum); // don't forget the last value.

	vector< pair<int,int> > outputPts;

	// Now generate random points
	srand((unsigned) time(0));
	double maxVal = cdf[cdf.size() - 1];
	int width = dst.cols;

	for (int i = 0; i < numPDFPoints; i++) {
		double rnd = float(rand()) / RAND_MAX;
		double threshold = rnd * maxVal;

		// We can speed this up with a binary search ...
		int idx = -1;

		int left = 0; int right = cdf.size();

		while (left < right) {
			int mid = (right - left) / 2 + left;
			double midVal = cdf[mid];

			if (midVal < threshold) { // Go right.
				left = mid + 1;
			} else {
				right = mid;
			}
		}

		// idx = left or right? I forget which?
		// (The difference is only 1 px).
		idx = left;

		int x = idx % width;
		int y = idx / width;

		outputPts.push_back(make_pair(x, y));
	}

	return outputPts;
}
