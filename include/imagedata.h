#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <string>



// This isn't indented to be a good representation;
// just a structure while refactoring.
// if it just maps to e.g. some OpenCV structure,
// then might as well remove it.
class ImageData {
public:
	ImageData(unsigned char *data, int width, int height);

	int width() {
		return loadedImageWidth;
	}

	int height() {
		return loadedImageHeight;
	}

	unsigned char* data() {
		return loadedImageData;
	}

	void dataAt(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b);

private:
	int loadedImageWidth;
	int loadedImageHeight;
	unsigned char *loadedImageData;
};

#endif // IMAGEDATA_H
