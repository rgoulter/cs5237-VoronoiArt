#include "generateProbabilityDistribution.h"

#include <stdlib.h>
#include <time.h>

#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "imagedata.h"
#include "platform.h"

using std::string;
using std::vector;

using cv::COLOR_BGR2GRAY;
using cv::Mat;
using cv::Size;
using cv::Scalar;
using cv::imread;



// OpenGL textures
extern GLuint edgesTexture;
extern GLuint edgesSharpTexture;
extern GLuint edgesBlurTexture;
extern GLuint pdfTexture;

extern string loadedImageFilename;
extern ImageData *imData;


Mat src, src_gray;
Mat dst, dst2, dst3, detected_edges, detected_edges2, detected_edges3;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int cannyRatio = 3;
int kernel_size = 3;



void generateOGLTextureForOpenCVMat(GLuint& tex, const Mat& M) {
	int loadedImageWidth = imData->width();
	int loadedImageHeight = imData->height();

	// copy the data to a new matrix
	Mat mat = M.clone();
	cvtColor(mat, mat, CV_GRAY2RGB);
	unsigned char *matData = (unsigned char*)(mat.data);

	// Now load the data into some opengl texture.
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_RGB,
	             loadedImageWidth,
	             loadedImageHeight,
	             0,
	             GL_RGB,
	             GL_UNSIGNED_BYTE,
	             matData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}



// It would be nice to abstract these details and have some kind of "constant" PDF / mat,
// and generate points from that. Oh well.
vector<int> generateUniformRandomPoints(int numPoints) {
	int loadedImageWidth = imData->width();
	int loadedImageHeight = imData->height();

	vector<int> outputPts;

	// Now generate random points
	srand((unsigned) time(0));

	for (int i = 0; i < numPoints; i++) {
		double rndX = double(rand()) / RAND_MAX;
		double rndY = double(rand()) / RAND_MAX;

		int x = (int) (rndX * loadedImageWidth);
		int y = (int) (rndY * loadedImageHeight);

		outputPts.push_back(x);
		outputPts.push_back(y);
	}

	return outputPts;
}



vector<int> generatePointsWithPDF(int numPDFPoints) {
	int loadedImageWidth = imData->width();

	src = imread(loadedImageFilename);

	if (!src.data) {
		return vector<int>();
	}

	// Create a matrix of the same type and size as src (for dst)
	dst.create(src.size(), src.type());

	// Convert the image to grayscale
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, Size(3,3));

	// Detect edges with Canny edge detector
	Canny(detected_edges, detected_edges, 100, 100*cannyRatio, kernel_size);

	// This is to get P_sharp?
	GaussianBlur(detected_edges, detected_edges2, Size(7,7), 0, 0);

	dst = Scalar::all(0);
	dst2 = Scalar::all(0);
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
	generateOGLTextureForOpenCVMat(edgesTexture, detected_edges);
	generateOGLTextureForOpenCVMat(edgesSharpTexture, detected_edges2);
	generateOGLTextureForOpenCVMat(edgesBlurTexture, dst3);
	generateOGLTextureForOpenCVMat(pdfTexture, dst);

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

	vector<int> outputPts;

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

		int x = idx % loadedImageWidth;
		int y = idx / loadedImageWidth;

		outputPts.push_back(x);
		outputPts.push_back(y);
	}

	return outputPts;

	// The following is kindof a mis-interpretation, I feel?
	// Doesn't look like what should be happening (e.g. magic 150??).

	/*
	unsigned char *input = (unsigned char*) (dst.data);
	vector<MyPoint> goodPoints;

	for(int i = 0; i < dst.cols; i++){
		for(int j = 0; j < dst.rows; j++){
			if((int)input[dst.cols * j + i] > 150){
				MyPoint point((LongInt)(i), (LongInt)(j));
				goodPoints.push_back(point);
			}
		}
	}

	// */
}
