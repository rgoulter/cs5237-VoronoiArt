#include "polypixel.h"
#include <string>

using namespace std;

// Use these from the main Qt application logic code..
extern int loadedImageWidth;
extern int loadedImageHeight;
extern unsigned char *loadedImageData;



vector<int> coercePSAPolyToIVecPoly(PointSetArray& psa){
	vector<int> poly;

	// Coerce psa to poly<int>
	for (int ptIdx = 1; ptIdx <= psa.noPt(); ptIdx++) {
		LongInt x, y;
		psa.getPoint(ptIdx, x, y);
		poly.push_back((int) x.doubleValue());
		poly.push_back((int) y.doubleValue()); 
	}

	return poly;
}

void boundingBox(const std::vector<int>& poly, int& minX, int& maxX, int& minY, int& maxY) {
	minX = loadedImageWidth;
	maxX = 0;
	minY = loadedImageHeight;
	maxY = 0;
	
	for (int i = 0; i < poly.size() - 1; i += 2) {
		int x = i;
		int y = i + 1;
		if(poly[x] < minX){ minX = poly[x]; }
		if(poly[x] > maxX){ maxX = poly[x]; }
		if(poly[y] < minY){ minY = poly[y]; }
		if(poly[y] > maxY){ maxY = poly[y]; }
	}
}

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



std::vector<int> enumerateLeftRightOfSimplePolygon(const std::vector<int>& poly) {
	std::vector<int> result;

	// Find bounding box of polygon
	int minX, maxX, minY, maxY;
	boundingBox(poly, minX, maxX, minY, maxY);


	// Within this bounding box, search for all the points.
	int left   = minX;
	int right  = maxX;
	int top    = minY;
	int bottom = maxY;
	
	for (int y = top; y <= bottom; y++) {
		int leftmost = -1, rightmost = -1;
		
		// Find leftmost point which is in the polygon
		for (int x = left; x <= right && leftmost < 0; x++) {
			if (inPoly(poly, x, y)) {
				leftmost = x;
			}
		}

		// Find rightmost point which is in polygon
		for (int x = right; x >= leftmost && rightmost < 0; x--) {
			if (inPoly(poly, x, y)) {
				rightmost = x;
			}
		}

		result.push_back(leftmost);
		result.push_back(rightmost);
	}
	
	// If we declare as a (const) ref, can we save ourselves
	// memory copying here?
	return result;
}

void findAverageColor3iv(const std::vector<MyPoint>& mpPoly, int* colorIv) {
	std::vector<int> poly;
	for (int i = 0; i < mpPoly.size(); i++) {
		poly.push_back((int) mpPoly[i].x.doubleValue());
		poly.push_back((int) mpPoly[i].y.doubleValue());
	}

	findAverageColor3iv(poly, colorIv);
}

void findAverageColor3iv(const std::vector<int>& poly, int* colorIv) {

	// Find bounding box of polygon
	int minX, maxX, minY, maxY;
	boundingBox(poly, minX, maxX, minY, maxY);

	// Within this bounding box, search for all the points.
	int left   = minX;
	int right  = maxX;
	int top    = minY;
	int bottom = maxY;



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
	
	
	// Get points in polygon.
	std::vector<int> rowsOfPoly = enumerateLeftRightOfSimplePolygon(poly);


	// For each point, find the average rgb.
	int accR, accG, accB;
	int acc = 0;
	int n = rowsOfPoly.size() / 2;

	// i = 0;
	int u, v;
	int rowLeft, rowRight;
	rowLeft = rowsOfPoly[0], rowRight = rowsOfPoly[1];
	u = rowLeft - offsetX;
	v = minY - offsetY;
	int pos = (v * width + u) * 3;
	accR = loadedImageData[pos], accG = loadedImageData[pos + 1], accB = loadedImageData[pos + 2];
	acc = 1;

	for(int j = rowLeft + 1; j <= rowRight; j++) {
		u = j - offsetX;
		v = minY - offsetY;

		int pos = (v * width + u) * 3;
		GLubyte rb = loadedImageData[pos + 0];
		GLubyte gb = loadedImageData[pos + 1];
		GLubyte bb = loadedImageData[pos + 2];
		
		// Accumulative average
		acc++;
		accR = ((acc - 1) * accR + rb) / acc;
		accG = ((acc - 1) * accG + gb) / acc;
		accB = ((acc - 1) * accB + bb) / acc;
	}

	for (int row = 1; row < n; row++) {
		rowLeft =  rowsOfPoly[row * 2];
		rowRight = rowsOfPoly[row * 2 + 1];
		
		for(int j = rowLeft + 1; j <= rowRight; j++) {
			u = j - offsetX;
			v = minY + row - offsetY;

			int pos = (v * width + u) * 3;
			GLubyte rb = loadedImageData[pos + 0];
			GLubyte gb = loadedImageData[pos + 1];
			GLubyte bb = loadedImageData[pos + 2];
		
			// Accumulative average
			acc++;
			accR = ((acc - 1) * accR + rb) / acc;
			accG = ((acc - 1) * accG + gb) / acc;
			accB = ((acc - 1) * accB + bb) / acc;
		}
	}

	colorIv[0] = accR;
	colorIv[1] = accG;
	colorIv[2] = accB;
}



void findSomeColor3iv(PointSetArray& psa, int* colorIv) {
	std::vector<int> poly = coercePSAPolyToIVecPoly(psa);
	findSomeColor3iv(poly, colorIv);
};



void findSomeColor3iv(const std::vector<int>& poly, int* colorIv) {

	// Find bounding box of polygon
	int minX, maxX, minY, maxY;
	boundingBox(poly, minX, maxX, minY, maxY);

	// Within this bounding box, search for all the points.
	int left   = minX;
	int right  = maxX;
	int top    = minY;
	int bottom = maxY;


	// Read the pixels from the relevant section
	int offsetX = 0;
	int offsetY = 0;
	int width  = loadedImageWidth; //right - left + 1;
	int height = loadedImageHeight; //bottom - top + 1;
	

	// For each point, find the average rgb.
	int accR, accG, accB;
	int acc = 0;
	int n = poly.size() / 2;

	// i = 0;
	int u, v;
	u = poly[0];
	v = poly[1];
	int pos = (v * width + u) * 3;
	accR = loadedImageData[pos], accG = loadedImageData[pos + 1], accB = loadedImageData[pos + 2];
	acc = 1;

	for (int ptIdx = 1; ptIdx < n; ptIdx++) {
		u = poly[2 * ptIdx];
		v = poly[2 * ptIdx + 1];

		int pos = (v * width + u) * 3;
		GLubyte rb = loadedImageData[pos + 0];
		GLubyte gb = loadedImageData[pos + 1];
		GLubyte bb = loadedImageData[pos + 2];
		
		// Accumulative average
		acc++;
		accR = ((acc - 1) * accR + rb) / acc;
		accG = ((acc - 1) * accG + gb) / acc;
		accB = ((acc - 1) * accB + bb) / acc;
	}

	colorIv[0] = accR;
	colorIv[1] = accG;
	colorIv[2] = accB;
}



bool isFileOrFolderInDir(string path, string filename) {
	return false;
}



bool findShaderDirectory(string& path, string shaderFilename) {
	path = "don't use this for now";
	return false;
}