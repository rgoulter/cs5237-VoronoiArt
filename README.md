# CS5237 Project.

Our group project for Computational Geometry.

Create a "stain-glass" effect using the Voronoi diagram of an image, with randomly generated points.

## Known Limitations and Future Improvements

* The color of the polygons do not represent the true average color of the area of the polygon in the image. This limitation was present for speed limitations. A GLSL fragment shader was implemented but not integrated.
* SOIL has issues loading certain images.
* Our implementation of Fortune's algorithm has difficulty with co-linear points. We jitter the input points in order to mitigate this.

## Visual Studio version

Solution and Project files are provided for MSVC++ 2010.

## Project Dependencies

Various libraries used in this project include:

* OpenGL
* OpenCV version 2.4.6
* Qt version 5.1.1
* GLEW (for shaders)
* SOIL (included in repo)
* GTest (for testing, included in repo)

## General Project Structure

Directories within the root project folder are:

* forms - for Qt .ui form files.
* gtest-1.7.0 - the gtest dependency.
* include - for .h, .hpp header files.
* make - for makefiles.
* msvc - for MSVC++ solutions and projects.
* resource - for Qt .rc files.
* shaders - for .vert, .frag GLSL shaders.
* soil - for the SOIL dependency.
* src - for .cpp source files.
* test - for .cpp gtest source files.

Within the include, src folders there is an expected hierarchy also:

basics, basicsP2, basicsP3 for historical reasons separate code that was made during phases I, II, and III. (This distinction is no longer necessary).

Folders linux, win32 are for platform-specific headers and source implementations; this avoids #ifdefs, and puts the onus of platform specific code on the build system.

### Visual Studio Projects

Projects include:

* basics_libs - for the core structures and algorithms used by the Qt application.
* cs5237_qt - the Qt application project.
* polyavg_shader_test - a small driver program to test speeds and results of methods to find color for a polygon.
* unittests - the GTest unit tests.
* gtest - the GTest dependency project.
* SOIL - the SOIL dependency project.

## GTest and such:
Maybe get Link Error2005, because GTest isn't quite right:
	"Make sure that all your individual projects are compiled with the same runtime libraries,
	this is specified in Properties -> C/C++ -> Code Generation -> Runtime Library...
	If you are using /MDd make sure that all other projects are too. Otherwise when you link
	them all together in the main project it will import multiple versions of the runtime libraries
	leading to the error you are observing.Make sure that all your individual projects are compiled
	with the same runtime libraries, this is specified in
	Properties -> C/C++ -> Code Generation -> Runtime Library...
	If you are using /MDd make sure that all other projects are too. Otherwise when you link them all
	together in the main project it will import multiple versions of the runtime libraries
	leading to the error you are observing."
http://stackoverflow.com/questions/921001/how-to-resolve-link-error-2005-in-visualstudio

And regarding Google Test, the following links maybe useful..
http://leefrancis.org/2010/11/17/google-test-gtest-setup-with-microsoft-visual-studio-2008-c/
http://www.cheezyworld.com/2010/12/19/using-google-test-with-visual-studio/

See also the Google Test Primer:
https://code.google.com/p/googletest/wiki/Primer