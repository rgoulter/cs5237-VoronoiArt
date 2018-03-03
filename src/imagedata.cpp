// the loaded image data was shamefully global.
// let's encapsulate that here.

#include "imagedata.h"

#include "opencv2/highgui/highgui.hpp"

using std::string;

using cv::Mat;
using cv::Rect;
using cv::imread;



// Assumes `mat` has RGB color.
Mat textureMatFromMat(const Mat& mat) {
	// Textures want width/height to be powers of 2.
	int imgW = mat.cols;
	int imgH = mat.rows;
	int texW = pow(2, ceil(log2(imgW)));
	int texH = pow(2, ceil(log2(imgH)));

	Mat textureMat = Mat(texH, texW, CV_8UC3);
	Mat region = textureMat(Rect(0, 0, imgW, imgH));

	if (mat.type() == region.type()) {
		mat.copyTo(region);
	} else if (mat.type() == CV_8UC1) {
		// AFAICT, this shouldn't happen, though.
		cvtColor(mat, region, CV_GRAY2RGB);
	} else {
		mat.convertTo(region, CV_8UC3, 255.0);
	}

	return textureMat;
}



// use the textureMat_ for this
void loadOpenGLTextureFromMat(const Mat& mat, GLuint *tex) {
	// See http://open.gl/textures for more information.

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);

	int width = mat.cols;
	int height = mat.rows;
	const unsigned char* data = mat.data;

	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             GL_RGB,
	             width,
	             height,
	             0,
	             GL_RGB,
	             GL_UNSIGNED_BYTE,
	             data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}



ImageData::ImageData(const Mat& inputMat, int codeToRGB) {
	cvtColor(inputMat, imageMat_, codeToRGB);

	textureMat_ = textureMatFromMat(imageMat_);

	loadOpenGLTextureFromMat(textureMat_, &texture_);
}



void ImageData::dataAt(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const {
	unsigned char *data = imageMat_.data;
	int pos = (y * imageMat_.cols + x) * 3;

	r = data[pos + 0];
	g = data[pos + 1];
	b = data[pos + 2];
}



void ImageData::renderPlane() const {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_);

	int width = imageMat_.cols;
	int height = imageMat_.rows;

	float texW = static_cast<float>(width) / textureWidth();
	float texH = static_cast<float>(height) / textureHeight();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, -1.0);

		glTexCoord2f(texW, 0.0);
		glVertex3f(width, 0.0, -1.0);

		glTexCoord2f(texW, texH);
		glVertex3f(width, height, -1.0);

		glTexCoord2f(0.0, texH);
		glVertex3f(0.0, height, -1.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}



// Uses OpenCV
ImageData* loadImageData(string imgFilename) {
	Mat imgMat = imread(imgFilename.c_str()); // BGR

	return new ImageData(imgMat, CV_BGR2RGB);
}
