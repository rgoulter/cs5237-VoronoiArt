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


