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
using cv::Mat;
using cv::Size;
using cv::Scalar;
using cv::imread;



// It would be nice to abstract these details and have some kind of "constant" PDF / mat,
// and generate points from that. Oh well.
//
// POINTREP:INTPAIRVEC
vector< pair<int,int> > generateUniformRandomPoints(int width, int height, int numPoints) {
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



// POINTREP:INTPAIRVEC
// TODO: generatePointsWithPDF could do with a tidyup
vector< pair<int,int> > generatePointsWithPDF(string imageFilename, int numPDFPoints, PDFTextures *oglTextures, int cannyRatio, int kernelSize) {
	Mat src = imread(imageFilename);

	int width = src.cols;

	if (!src.data) {
		return vector< pair<int,int> >();
	}

	// Create a matrix of the same type and size as src (for dst)
	Mat dst(src.size(), src.type());

	// Convert the image to grayscale
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	// Reduce noise with a kernel 3x3
	Mat detected_edges;
	blur(src_gray, detected_edges, Size(3,3));

	// Detect edges with Canny edge detector
	Canny(detected_edges, detected_edges, 100, 100*cannyRatio, kernelSize);

	// This is to get P_sharp?
	Mat detected_edges2;
	GaussianBlur(detected_edges, detected_edges2, Size(7,7), 0, 0);

	dst = Scalar::all(0);
	Mat dst2;
	dst2 = Scalar::all(0);
	Mat dst3;
	dst3 = Scalar::all(0);

	src_gray.copyTo(dst2, detected_edges2);
	src_gray.copyTo(dst, detected_edges);

	// This is to get P_blur?
	GaussianBlur(dst2, dst3, Size(15,15), 0, 0);
	dst3.convertTo(dst3, -1, 2, 0);

	// Ps = Pblur - Psharp
	subtract(dst3, dst2, dst);


	// Make OpenGL Textures for the following:
	// (I'm not 100% certain about these mappings?).
	if (oglTextures != NULL) {
		oglTextures->edgesTexture = new ImageData(detected_edges, CV_GRAY2RGB);
		oglTextures->edgesSharpTexture = new ImageData(detected_edges2, CV_GRAY2RGB);
		oglTextures->edgesBlurTexture = new ImageData(dst3, CV_GRAY2RGB);
		oglTextures->pdfTexture = new ImageData(dst, CV_GRAY2RGB);
	}

	// So, let's build up a 1-D array of the CDF from the 2D PDF.
	//  idx -> (row, col) : row = idx / width, col = idx % width
	//  (row, col) -> idx : idx = row * width + col


	// use float or uchar?
	vector<float> cdf; //loadedImageWidth * loadedImageHeight
	float sum = 0;

	for (int r = 0; r < dst.rows; r++) {
		for (int c = 0; c < dst.cols; c++) {
			unsigned char val = dst.at<unsigned char> (r, c);

			cdf.push_back(sum);
			sum += (float) val / 255;
		}
	}

	cdf.push_back(sum); // don't forget the last value.

	vector< pair<int,int> > outputPts;

	// Now generate random points
	srand((unsigned) time(0));
	double maxVal = cdf[cdf.size() - 1];

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

		outputPts.push_back(make_pair(x,y));
	}

	return outputPts;
}

