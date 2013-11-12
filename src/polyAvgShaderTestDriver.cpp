// polyAvgShaderTestDriver.cpp - for checking the poly-avg-color shader and things.

#include <cstdio>
#include <iostream>
#include <string>

#include "polypixel.h"

using namespace std;

// PolyPixel requires us to have these; it'd be nice to do this some other way.
int loadedImageWidth;
int loadedImageHeight;
unsigned char *loadedImageData;

int main(int argc, char** argv) {
	string shaderDir;

	string shaderName = "reduce.frag";
	bool found = findShaderDirectory(shaderDir, shaderName);

	if (found) {
		cout << "Shader directory " << shaderDir << endl;
	} else {
		cout << "Could not find shader directory." << endl;
	}
	getchar();
	return 0;
}