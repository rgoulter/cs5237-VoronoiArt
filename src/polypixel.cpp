#include "polypixel.h"

#include <assert.h>

#include <iostream>
#include <string>
#include <utility>

#include "stopwatch.h"

using std::pair;
using std::string;
using std::vector;
using std::cout;
using std::endl;

using geometry::Polygon;
using geometry::Rect;
using geometry::boundingBox;
using geometry::clipPolygonToRectangle;
using geometry::inPoly;



// also uses polypixel's ColoredPolygon
vector<ColoredPolygon> generateColoredPolygons(const vector<geometry::Polygon>& polys, const ImageData& imData) {
	vector<ColoredPolygon> renderedPolygons;

	StopWatch allSW;
	allSW.resume();

	vector<geometry::Polygon> clippedPolys;

	// Clip polygon to ensure we have nothing out of bounds
	int loadedImageWidth = imData.width();
	int loadedImageHeight = imData.height();
	Rect loadedImageRect({0, 0}, loadedImageWidth - 1, loadedImageHeight - 1);

	for (const geometry::Polygon& poly : polys) {
		const geometry::Polygon& clippedPoly = clipPolygonToRectangle(poly, loadedImageRect);

		if (clippedPoly.numPoints() > 0) {
			clippedPolys.push_back(clippedPoly);
		}
	}


	for (const geometry::Polygon& poly : clippedPolys) {
		// TODO: Would be nice to be able to inject another function
		// instead of `findSomeColor3iv`.
		int colorIv[3];
		findSomeColor3iv(imData, poly, colorIv);

		// XXX following could be a method / ctor, right?
		ColoredPolygon coloredPoly;

		coloredPoly.poly = poly;
		coloredPoly.rgb[0] = static_cast<float>(colorIv[0]) / 255;
		coloredPoly.rgb[1] = static_cast<float>(colorIv[1]) / 255;
		coloredPoly.rgb[2] = static_cast<float>(colorIv[2]) / 255;

		renderedPolygons.push_back(coloredPoly);
	}

	allSW.pause();
	double timeFindSomeColor = allSW.ms();
	int n = polys.size();
	double timeAvg = timeFindSomeColor / n;
	cout << "TIME: Average: " << timeAvg << " for " << n << " polygons. Total: " << timeFindSomeColor << endl;

	return renderedPolygons;
}



// TODO: enumerateLROfSimplePoly would be clearer as a list of pairs.
// TODO: there's probably a more effecient geometry algo. to compute this.
vector<pair<int,int>> enumerateLeftRightOfSimplePolygon(const geometry::Polygon& poly) {
	vector<pair<int,int>> result;

	// Find bounding box of polygon
	// COORD CONFUSION again. we're using top() ~ y1
	const Rect& boundingRect = boundingBox(poly);
	int minX = boundingRect.left();
	int maxX = boundingRect.left();
	int minY = boundingRect.top();
	int maxY = boundingRect.bottom();


	// Within this bounding box, search for all the points.
	int left   = minX;
	int right  = maxX;
	int top    = minY;
	int bottom = maxY;

	for (int y = top; y <= bottom; y++) {
		int leftmost = -1, rightmost = -1;

		// Find leftmost point which is in the polygon
		for (int x = left; x <= right && leftmost < 0; x++) {
			if (inPoly(poly, {x, y})) {
				leftmost = x;
			}
		}

		// Find rightmost point which is in polygon
		for (int x = right; x >= leftmost && rightmost < 0; x--) {
			if (inPoly(poly, {x, y})) {
				rightmost = x;
			}
		}

		result.push_back({leftmost, rightmost});
	}

	// If we declare as a (const) ref, can we save ourselves
	// memory copying here?
	return result;
}



void findAverageColor3iv(const ImageData& imData, const geometry::Polygon& poly, int* colorIv) {
	// Find bounding box of polygon
	const Rect& boundingRect = boundingBox(poly);
	int minX = boundingRect.left();
	int maxX = boundingRect.left();
	int minY = boundingRect.top();
	int maxY = boundingRect.bottom();

	// Within this bounding box, search for all the points.
	// int left   = minX;
	// int right  = maxX;
	// int top    = minY;
	// int bottom = maxY;



	// Read the pixels from the relevant section
	/*int offsetX = left;
	int offsetY = top;
	int width = right - left + 1;
	int height = bottom - top + 1;*/
	int offsetX = 0;
	int offsetY = 0;
	// int width  = loadedImageWidth; //right - left + 1;
	// int height = loadedImageHeight; //bottom - top + 1;

	// TODO: Read values from texture itself, so we can save memory


	// Get points in polygon.
	vector<pair<int,int>> rowsOfPoly = enumerateLeftRightOfSimplePolygon(poly);


	// For each point, find the average rgb.
	unsigned char accR, accG, accB;
	int acc = 0;
	int n = rowsOfPoly.size();

	// i = 0;
	int rowLeft = rowsOfPoly[0].first;
	int rowRight = rowsOfPoly[0].second;
	int u = rowLeft - offsetX;
	int v = minY - offsetY;
	imData.dataAt(u, v, accR, accG, accB);
	acc = 1;

	for(int j = rowLeft + 1; j <= rowRight; j++) {
		u = j - offsetX;
		v = minY - offsetY;

		GLubyte rb, gb, bb;
		imData.dataAt(u, v, rb, gb, bb);

		// Accumulative average
		acc++;
		accR = ((acc - 1) * accR + rb) / acc;
		accG = ((acc - 1) * accG + gb) / acc;
		accB = ((acc - 1) * accB + bb) / acc;
	}

	for (int row = 1; row < n; row++) {
		rowLeft =  rowsOfPoly[row].first;
		rowRight = rowsOfPoly[row].second;

		for (int j = rowLeft + 1; j <= rowRight; j++) {
			u = j - offsetX;
			v = minY + row - offsetY;

			GLubyte rb, gb, bb;
			imData.dataAt(u, v, rb, gb, bb);

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



void findSomeColor3iv(const ImageData& imData, const geometry::Polygon& poly, int* colorIv) {
	int loadedImageWidth = imData.width();
	int loadedImageHeight = imData.height();

	assert(poly.numPoints() > 0);

	// Read the pixels from the relevant section
	// int offsetX = 0;
	// int offsetY = 0;
	// int width  = loadedImageWidth; //right - left + 1;
	// int height = loadedImageHeight; //bottom - top + 1;


	// For each point, find the average rgb.
	unsigned char accR, accG, accB;
	int acc = 0;
	int n = poly.numPoints();

	// i = 0;
	int u = poly[0].x;
	int v = poly[0].y;

	if (u < 0 || u > loadedImageWidth ||
	    v < 0 || v > loadedImageHeight) {
		// This only happens in the POLYGON WASN'T TRIMMED.
		cout << "BAD! Polygon not trimmed! (1)" << endl;
		cout << "Image width: " << loadedImageWidth << ", " << loadedImageHeight << endl;
		cout << "Poly points:" << endl;
		for (const geometry::Point<int>& pt : poly.points()) {
			cout << "  " << pt << endl;
		}
		colorIv[0] = 0;
		colorIv[1] = 0;
		colorIv[2] = 0;
		return;
	}

	// GLubyte rb, gb, bb;
	imData.dataAt(u, v, accR, accG, accB);
	acc = 1;

	for (int ptIdx = 1; ptIdx < n; ptIdx++) {
		u = poly[ptIdx].x;
		v = poly[ptIdx].y;

		if (u < 0 || u > loadedImageWidth ||
		    v < 0 || v > loadedImageHeight) {
			// This only happens in the POLYGON WASN'T TRIMMED.
			cout << "BAD! Polygon not trimmed! (2)" << endl;
			cout << "Image width: " << loadedImageWidth << ", " << loadedImageHeight << endl;
			cout << "Poly points:" << endl;
			for (const geometry::Point<int>& pt : poly.points()) {
				cout << "  " << pt << endl;
			}
			colorIv[0] = 0;
			colorIv[1] = 0;
			colorIv[2] = 0;
			return;
		}



		GLubyte rb, gb, bb;
		imData.dataAt(u, v, rb, gb, bb);

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



// bool isFileOrFolderInDir(string path, string filename)
bool isFileOrFolderInDir(string, string) {
	return false;
}



// bool findShaderDirectory(string& path, string shaderFilename)
bool findShaderDirectory(string& path, string) {
	path = "don't use this for now";
	return false;
}

