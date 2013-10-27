#include "polypixel.h"

// Use these from the main Qt application logic code..
extern int loadedImageWidth;
extern int loadedImageHeight;
extern unsigned char *loadedImageData;

void boundingBox(const std::vector<MyPoint>& poly, LongInt& minX, LongInt& maxX, LongInt& minY, LongInt& maxY) {
	minX = loadedImageWidth;
	maxX = 0;
	minY = loadedImageHeight;
	maxY = 0;
	
	for (int i = 0; i < poly.size(); i++) {
		if(poly[i].x < minX){ minX = poly[i].x; }
		if(poly[i].x > maxX){ maxX = poly[i].x; }
		if(poly[i].y < minY){ minY = poly[i].y; }
		if(poly[i].y > maxY){ maxY = poly[i].y; }
	}
}

PointSetArray enumeratePixelsInPolygon(const std::vector<MyPoint>& poly) {
	PointSetArray psa;

	// Find bounding box of polygon
	LongInt minX, maxX, minY, maxY;
	boundingBox(poly, minX, maxX, minY, maxY);


	// Within this bounding box, search for all the points.
	int left   = (int) minX.doubleValue();
	int right  = (int) maxX.doubleValue();
	int top    = (int) minY.doubleValue();
	int bottom = (int) maxY.doubleValue();

	for (int x = left; x <= right; x++) {
		for (int y = top; y <= bottom; y++) {
			// Can improve this? e.g. we construct two MyPoints here.
			if (inPoly(poly, MyPoint(x, y))) {
				psa.addPoint(x, y);
			}
		}
	}
	
	// If we declare as a (const) ref, can we save ourselves
	// memory copying here?
	return psa;
}

void findAverageColor3iv(GLuint glTex, const std::vector<MyPoint>& poly, int* colorIv) {
	// Find bounding box of polygon
	LongInt minX, maxX, minY, maxY;
	boundingBox(poly, minX, maxX, minY, maxY);

	// Within this bounding box, search for all the points.
	int left   = (int) minX.doubleValue();
	int right  = (int) maxX.doubleValue();
	int top    = (int) minY.doubleValue();
	int bottom = (int) maxY.doubleValue();



	// Read the pixels from the relevant section
	/*int offsetX = left;
	int offsetY = top;
	int width = right - left + 1;
	int height = bottom - top + 1;*/
	int offsetX = 0;
	int offsetY = 0;
	int width  = loadedImageWidth; //right - left + 1;
	int height = loadedImageHeight; //bottom - top + 1;
	
	// TODO: Read values from texture itself, so we can save memory
	

	// TODO: At the moment, the following is SLOW.
	// PointSetArray pointsInPoly = enumeratePixelsInPolygon(poly);


	// For each point, find the average rgb.
	int accR, accG, accB;
	int n = poly.size();

	// i = 0;
	MyPoint p = poly[0];
	int u = ((int) p.x.doubleValue()) - offsetX;
	int v = ((int) p.y.doubleValue()) - offsetY;
	int pos = (v * width + u) * 3;
	accR = loadedImageData[pos], accG = loadedImageData[pos + 1], accB = loadedImageData[pos + 2];

	for (int i = 1; i < n; i++) {
		MyPoint p = poly[i];
		int u = ((int) p.x.doubleValue()) - offsetX;
		int v = ((int) p.y.doubleValue()) - offsetY;

		// Get pixel from the data
		int pos = (v * width + u) * 3;
		GLubyte rb = loadedImageData[pos + 0];
		GLubyte gb = loadedImageData[pos + 1];
		GLubyte bb = loadedImageData[pos + 2];
		
		// Accumulative average
		accR = (i - 1) * accR / i + rb / i;
		accG = (i - 1) * accG / i + gb / i;
		accB = (i - 1) * accB / i + bb / i;
	}

	colorIv[0] = accR;
	colorIv[1] = accG;
	colorIv[2] = accB;
}