// polyAvgShaderTestDriver.cpp - for checking the poly-avg-color shader and things.

#include <cstdio>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "polypixel.h"

#include "shader_util.h"
#include "stopWatch.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS & GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

// PolyPixel requires us to have these; it'd be nice to do this some other way.
int loadedImageWidth;
int loadedImageHeight;
unsigned char *loadedImageData;


// Size of input matrix.
const int matRows = 2047;
const int matCols = 257;

// Reduction factor.
// During the reduction operation, reduceFact x reduceFactor elements
// in the input texture are reduced to 1 element for the output.
const int reduceFact = 3;

// Shaders' filenames.
const char vertShaderFilename[] = "reduce.vert";
const char fragShaderFilename[] = "reduce.frag";

// Shader program object.
GLuint shaderProg = 0;

// GLUT window handle.
GLuint glutWindowHandle;



/////////////////////////////////////////////////////////////////////////////
// Initialize the data.
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// Free the memory allocated to the data.
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// Compute on the CPU.
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// Check framebuffer status.
// Modified from the sample code provided in the 
// GL_EXT_framebuffer_object extension sepcifications.
/////////////////////////////////////////////////////////////////////////////
bool CheckFramebufferStatus() {
    GLenum status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            fprintf(stderr, "Framebuffer incomplete, incomplete attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            fprintf(stderr, "Framebuffer incomplete, attached images must have same dimensions\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            fprintf(stderr, "Framebuffer incomplete, attached images must have same format\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing draw buffer\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing read buffer\n");
            return false;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            fprintf(stderr, "Unsupported framebuffer format\n");
            return false;
    }

    return false;
}



/////////////////////////////////////////////////////////////////////////////
// Compute on the GPU.
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// Generate data.
/////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// Create a OpenGL rendering context. 
// Check for OpenGL 2.0 and the necessary OpenGL extensions. 
// Read in the shaders from files to create a shader program object.
/////////////////////////////////////////////////////////////////////////////
void PrepareGPUExecution(int argc, char** argv) {
	// Initialize GLUT.
    glutInit(&argc, argv);

    // This creates a OpenGL rendering context so that
    // we can start to issue OpenGL commands after this.
    glutWindowHandle = glutCreateWindow("");  
 
	// Initialize GLEW.
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s.\n", glewGetErrorString(err));
        char ch; scanf("%c", &ch); // Prevents the console window from closing.
        exit(1);
    }

	// Make sure OpenGL 2.0 is supported.
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: OpenGL 2.0 is not supported.\n");
        char ch;
		scanf("%c", &ch); // Prevents the console window from closing.
        exit(1);
    }

	// Make sure necessary OpenGL extensions are supported.
    if (!GLEW_ARB_texture_float || 
        !GLEW_EXT_framebuffer_object || 
        !GLEW_ARB_texture_rectangle ) {
        fprintf(stderr, "Error: Some necessary OpenGL extensions are not supported.\n");
        char ch; scanf("%c", &ch); // Prevents the console window from closing.
        exit(1);
    }

	// Create shader program object.
    shaderProg = makeShaderProgramFromFiles(vertShaderFilename, fragShaderFilename, NULL);
    if (shaderProg == 0) {
        fprintf(stderr, "Error: Cannot create shader program object.\n");
        char ch; scanf("%c", &ch); // Prevents the console window from closing.
        exit(1);
    }
}



/////////////////////////////////////////////////////////////////////////////
// The main function.
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
    double starttime, endtime;

    srand(927); // To use same set of random numbers every run.

//-----------------------------------------------------------------------------
// Initialise data
//-----------------------------------------------------------------------------
    

//-----------------------------------------------------------------------------
// Perform computation on GPU.
//-----------------------------------------------------------------------------
    printf("GPU COMPUTATION:\n");

	PrepareGPUExecution(argc, argv);
	
    starttime = 0; // TODO: Stopwatch

    // GPU Calculation here.

    endtime = -1; // TODO: Stopwatch
    printf("Time elapsed = %.4f sec\n", endtime - starttime);

    // Print some results.
    printf("Result = ????\n");
    printf("\n\n");

//-----------------------------------------------------------------------------
// Perform computation on CPU.
//-----------------------------------------------------------------------------
    printf("CPU COMPUTATION:\n");
    starttime = 0; // TODO: Stopwatch

    // CPU Calculation here.

    endtime = -1;
    printf("Time elapsed = %.4f sec\n", endtime - starttime);

    // Print some results.
    printf("Result = ????\n");
    printf("\n\n");

//-----------------------------------------------------------------------------
// Clean up.
//-----------------------------------------------------------------------------	
    
    char ch; scanf("%c", &ch); // Prevents the console window from closing.
    return 0;
}