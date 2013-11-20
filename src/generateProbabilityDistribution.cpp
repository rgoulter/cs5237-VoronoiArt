#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "platform.h"

using namespace cv;



// OpenGL textures
extern GLuint edgesTexture;
extern GLuint edgesSharpTexture;
extern GLuint edgesBlurTexture;
extern GLuint pdfTexture;

extern string loadedImageFilename;
extern int loadedImageWidth;
extern int loadedImageHeight;


Mat src, src_gray;
Mat dst, dst2, dst3, detected_edges, detected_edges2, detected_edges3;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;



void generateOGLTextureForOpenCVMat(GLuint& tex, const Mat& M){
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



void generatePointsWithPDF() {
	src = imread(loadedImageFilename);

	if (!src.data) {
		return;
	}

	// Create a matrix of the same type and size as src (for dst)
	dst.create(src.size(), src.type());

	// Convert the image to grayscale
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

    // Reduce noise with a kernel 3x3
    blur(src_gray, detected_edges, Size(3,3));

    // Detect edges with Canny edge detector
    Canny(detected_edges, detected_edges, 100, 100*ratio, kernel_size);
	
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

	srand((unsigned) time(0));

    int random_point;
	int lowest = 0, highest = goodPoints.size()-1;
    int range = (highest - lowest) + 1;
    for(int index = 0; index < numPDFPoints; index++){
        random_point = lowest + int(range * rand() / (RAND_MAX + 1.0));
		tryInsertPoint(goodPoints[random_point].x, goodPoints[random_point].y);
    }
	// */
}