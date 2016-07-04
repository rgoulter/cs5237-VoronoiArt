#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <string>



// This isn't indented to be a good representation;
// just a structure while refactoring.
// if it just maps to e.g. some OpenCV structure,
// then might as well remove it.
class ImageData {
public:
	ImageData(unsigned char *data,
	          unsigned char *texData,
	          int width,
	          int height,
	          int texWidth,
	          int texHeight) :
	  imageData_(data),
	  textureData_(texData),
	  imageWidth_(width),
	  imageHeight_(height),
	  texWidth_(texWidth),
	  texHeight_(texHeight)
	{
	}

	int width() const {
		return imageWidth_;
	}

	int height() const {
		return imageHeight_;
	}

	int textureWidth() const {
		return texWidth_;
	}

	int textureHeight() const {
		return texHeight_;
	}

	unsigned char* data() {
		return imageData_;
	}

	unsigned char* textureData() {
		return textureData_;
	}

	void dataAt(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b) const;

private:
	int imageWidth_;
	int imageHeight_;
	int texWidth_;
	int texHeight_;
	unsigned char *imageData_;
	unsigned char *textureData_;
};

#endif // IMAGEDATA_H
