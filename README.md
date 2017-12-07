# CS5237 Project.

[![Build Status](https://travis-ci.org/rgoulter/cs5237-VoronoiArt.svg?branch=master)](https://travis-ci.org/rgoulter/cs5237-VoronoiArt)

Create a "stain-glass" effect using the Voronoi diagram of an image, with
randomly generated points.

Our group project for Computational Geometry CS5237 course in NUS.

This project is based on methods out in the paper:
<http://www.cs.swarthmore.edu/~adanner/cs97/s08/papers/gorbach.pdf>

## Usage

Load an image using the "..." button.

Select a number of points to generate. 1,000 works well.

Generate random points, either with a uniform distribution, or with a
Probability Distribution based on detected edges.

Generate the Voronoi diagram of these points.

Clicking the "Draw Image", "Draw Effect" buttons will allow you to switch
between what is shown on the canvas. Voronoi sites and edges can be
shown/hidden with the checkboxes.  
(You can also see the Edge detection steps, if the points were generated with
the edge detection method).

Clicking "Save Image" will output the canvas to "output.bmp" in the current
directory.

## Known Limitations and Future Improvements

* The color of the polygons do not represent the true average color of the area
  of the polygon in the image. This limitation was present for speed
  limitations. A GLSL fragment shader was implemented but not integrated.
* Our implementation of Fortune's algorithm has difficulty with co-linear
  points. We jitter the input points in order to mitigate this.

## Project Dependencies

Various libraries used in this project include:

<!-- XXX Links to download sections for these.. -->
* OpenGL
* OpenCV version 2.4
* Qt 5
* GTest (for testing, included in repo)

## Compiling on Linux

On Ubuntu/Linux, it's fairly easy to get the above dependencies:
```sudo apt-get install libopencv-dev qt5-default libqt5-opengl5-dev```
should be enough to get all the required dependencies listed above.

Use CMake to generate a Makefile.
e.g.

```
mkdir build
cd build
cmake -Dtest=ON ..
make
```

## General Project Structure

Directories within the root project folder are:

* forms - for Qt .ui form files.
* gtest-1.7.0 - the gtest dependency.
* include - for .h, .hpp header files.
* make - for makefiles.
* resource - for Qt .rc files.
* shaders - for .vert, .frag GLSL shaders.
* src - for .cpp source files.
* test - for .cpp gtest source files.

Within the include, src folders there is an expected hierarchy also:

Folders linux, win32 are for platform-specific headers and source
implementations; this avoids #ifdefs, and puts the onus of platform specific
code on the build system.

