// the loaded image data was shamefully global.
// let's encapsulate that here.

#include "imagedata.h"



void ImageData::dataAt(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const {
	int pos = (y * this->imageWidth_ + x) * 3;
	r = this->imageData_[pos + 0];
	g = this->imageData_[pos + 1];
	b = this->imageData_[pos + 2];
}

