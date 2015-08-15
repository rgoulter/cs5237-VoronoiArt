// the loaded image data was shamefully global.
// let's encapsulate that here.

#include "imagedata.h"

// int loadedImageWidth = -1;
// int loadedImageHeight = -1;
// unsigned char *loadedImageData;

ImageData::ImageData(unsigned char *data, int width, int height) {
	this->loadedImageData = data;
	this->loadedImageWidth = width;
	this->loadedImageHeight = height;
}


void ImageData::dataAt(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) {
	int pos = (y * this->loadedImageWidth + x) * 3;
	r = this->loadedImageData[pos + 0];
	g = this->loadedImageData[pos + 1];
	b = this->loadedImageData[pos + 2];
}
