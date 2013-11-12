#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32 /*[*/
#include <io.h>
#endif /*]*/

#include <GL/glew.h>

#include "shader_util.h"



/////////////////////////////////////////////////////////////////////////////
// Print OpenGL error if there is any.
// Returns 1 if an OpenGL error occurred, 0 otherwise.
/////////////////////////////////////////////////////////////////////////////
int printOglError( char *file, int line )
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError 0x%x file %s @ %d: %s.\n", glErr, file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}



/////////////////////////////////////////////////////////////////////////////
// Print out the information log for a shader object.
/////////////////////////////////////////////////////////////////////////////
static void printShaderInfoLog( GLuint shader )
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    printOpenGLError();  // Check for OpenGL errors.

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

    printOpenGLError();  // Check for OpenGL errors.

    if (infologLength > 0)
    {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer.\n");
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
        if (strlen(infoLog) > 0) printf("Shader InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    printOpenGLError();  // Check for OpenGL errors.
}



/////////////////////////////////////////////////////////////////////////////
// Print out the information log for a program object.
/////////////////////////////////////////////////////////////////////////////
static void printProgramInfoLog( GLuint program )
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    printOpenGLError();  // Check for OpenGL errors.

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

    printOpenGLError();  // Check for OpenGL errors.

    if (infologLength > 0)
    {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer.\n");
            exit(1);
        }
        glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
        if (strlen(infoLog) > 0) printf("Program InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    printOpenGLError();  // Check for OpenGL errors.
}



/////////////////////////////////////////////////////////////////////////////
// Returns the size in bytes of the file fileName.
// If an error occurred, it returns -1.
/////////////////////////////////////////////////////////////////////////////
static int fileSize( const char *fileName )
{
    int fd;
    int count = -1;

    // Open the file, seek to the end to find its length.

#ifdef _WIN32
    fd = _open(fileName, _O_RDONLY);
    if (fd != -1)
    {
        count = _lseek(fd, 0, SEEK_END);
        _close(fd);
    }
#else
    fd = open(name, O_RDONLY);
    if (fd != -1)
    {
        count = lseek(fd, 0, SEEK_END);
        close(fd);
    }
#endif

    return count;
}


/////////////////////////////////////////////////////////////////////////////
// Reads a shader from the supplied file and returns the shader in the
// array passed in. Returns 1 if successful, 0 if an error occurred.
// The parameter size is an upper limit of the amount of bytes to read.
// It is ok for it to be too big.
/////////////////////////////////////////////////////////////////////////////
static int readShader( const char *fileName, char *shaderText, int size )
{
    FILE *fh;
    int count;

    // Open the file.
    fh = fopen(fileName, "r");
    if (!fh) return 0;

    // Get the shader from a file.
    fseek(fh, 0, SEEK_SET);
    count = (int) fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh)) count = 0;

    fclose(fh);
    return count;
}



/////////////////////////////////////////////////////////////////////////////
// Reads shader source code from file fileName. 
// It allocates memory for the null-terminated text string, 
// pointed to by *shaderSource.
// Returns 0 on error, otherwise return 1.
/////////////////////////////////////////////////////////////////////////////
int readShaderSource( const char *fileName, GLchar **shaderSource )
{
    // Allocate memory to hold the source of the shader.

    int fsize = fileSize(fileName);

    if (fsize == -1)
    {
		printf("ERROR: Cannot determine size of the shader %s.\n", fileName);
        return 0;
    }

    *shaderSource = (GLchar *) malloc(fsize + 1);  // Extra byte for null character.
    if (*shaderSource == NULL)
    {
        printf("ERROR: Cannot allocate memory for shader source.\n");
        return 0;
    }

    // Read the source code.

    if (!readShader(fileName, *shaderSource, fsize + 1))
    {
        printf("ERROR: Cannot read the file %s.\n", fileName);
        return 0;
    }

    return 1;
}



/////////////////////////////////////////////////////////////////////////////
// Get the location of a uniform variable.
/////////////////////////////////////////////////////////////////////////////
GLint getUniLoc( GLuint program, const GLchar *name )
{
    GLint loc;

    loc = glGetUniformLocation(program, name);

    if (loc == -1)
        printf("ERROR: No such uniform named \"%s\".\n", name);

    printOpenGLError();  // Check for OpenGL errors.
    return loc;
}



/////////////////////////////////////////////////////////////////////////////
// Create and compile a program object from the input shader source strings.
// Returns 0 on error, otherwise returns a program object ID.
// Either vertShaderSrcStr or fragShaderSrcStr can be NULL.
//
// The parameter bindAttribLocFunc is a pointer to a user-defined 
// function that assigns generic attribute locations/indices to 
// attribute variable names declared in the vertex shader. 
// bindAttribLocFunc can be NULL, and in this case, the linker
// will assign the locations/indices automatically. 
// An example of such a function:
//
//		void myBindAttribLocFunc( GLuint prog )
//		{    
//			glBindAttribLocation( prog, 1, "myAttrib_A" );
//			printOpenGLError();
//			glBindAttribLocation( prog, 2, "myAttrib_B" );
//			printOpenGLError();
//		}

// On successful return, the returned program object ID can be 
// used to install the program as part of current state 
// using glUseProgram(). Example:
//
//     GLuint prog = makeShaderProgram( vertShaderStr, fragShaderStr,
//                                      myBindAttribLocFunc );
//     if ( prog != 0 ) 
//     {
//         glUseProgram( prog );
//
//         // Set values of uniform variables only 
//         // after the prog has become current.
//         glUniform3f( getUniLoc(prog, "LightPosition"), 10.0, 10.0, 15.0 );
//         printOpenGLError();
//         glUniform1f( getUniLoc(prog, "Density"), 16.0 );
//         printOpenGLError();
//     }
// 
// Use glUseProgram(0) to restore the original vertex processing 
// and fragement processing fixed functionalities.
//
/////////////////////////////////////////////////////////////////////////////
GLuint makeShaderProgram( const GLchar *vertShaderSrcStr, 
						  const GLchar *fragShaderSrcStr,
						  void (*bindAttribLocFunc)( GLuint progObj ) )
{
	GLuint vShader = 0, fShader = 0, prog = 0;  // handles to objects.
	GLint  vCompiled, fCompiled;  // status values.
	GLint  linked;

	// Create and compile the vertex shader object.
	if ( vertShaderSrcStr != NULL )
	{
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vertShaderSrcStr, NULL);
		glCompileShader(vShader);
		printOpenGLError();  // Check for OpenGL errors.
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &vCompiled);
		printShaderInfoLog(vShader);
		if (!vCompiled ) return 0;
	}

	// Create and compile the fragment shader object.
	if ( fragShaderSrcStr != NULL )
	{
		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fragShaderSrcStr, NULL);
		glCompileShader(fShader);
		printOpenGLError();  // Check for OpenGL errors.
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &fCompiled);
		printShaderInfoLog(fShader);
		if (!fCompiled ) return 0;
	}

	// Create a program object and attach the two compiled shaders.
	prog = glCreateProgram();
	if ( vertShaderSrcStr != NULL ) glAttachShader(prog, vShader);
	if ( fragShaderSrcStr != NULL ) glAttachShader(prog, fShader);

	// Flag shaders for deletion. 
	// They will only be actually deleted when the program object is deleted.
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	// If bindAttribLocFunc != NULL, then
	// use a user-provided function to assign generic attribute 
	// locations/indicses to generic attribute names,
	// otherwise, just let the linker do the assignment automatically.
	if ( bindAttribLocFunc != NULL ) bindAttribLocFunc( prog );

	// Link the program object.
	glLinkProgram(prog);
    printOpenGLError();  // Check for OpenGL errors.
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    printProgramInfoLog(prog);

	if (!linked) return 0;

	return prog;
} 



/////////////////////////////////////////////////////////////////////////////
// Same as makeShaderProgram(), except the first two parameters are
// now the filenames of the shader source code.
//
// Create and compile a program object from the input shader source files.
// Returns 0 on error, otherwise returns a program object ID.
// Either vertShaderSrcFilename or fragShaderSrcFilename can be NULL.
//
// The parameter bindAttribLocFunc is a pointer to a user-defined 
// function that assigns generic attribute locations/indices to 
// attribute variable names declared in the vertex shader. 
// bindAttribLocFunc can be NULL, and in this case, the linker
// will assign the locations/indices automatically. 
// An example of such a function:
//
//		void myBindAttribLocFunc( GLuint prog )
//		{    
//			glBindAttribLocation( prog, 1, "myAttrib_A" );
//			printOpenGLError();
//			glBindAttribLocation( prog, 2, "myAttrib_B" );
//			printOpenGLError();
//		}

// On successful return, the returned program object ID can be 
// used to install the program as part of current state 
// using glUseProgram(). Example:
//
//     GLuint prog = makeShaderProgramFromFiles( vertShaderFile, fragShaderFile,
//                                               myBindAttribLocFunc );
//     if ( prog != 0 ) 
//     {
//         glUseProgram( prog );
//
//         // Set values of uniform variables only 
//         // after the prog has become current.
//         glUniform3f( getUniLoc(prog, "LightPosition"), 10.0, 10.0, 15.0 );
//         printOpenGLError();
//         glUniform1f( getUniLoc(prog, "Density"), 16.0 );
//         printOpenGLError();
//     }
// 
// Use glUseProgram(0) to restore the original vertex processing 
// and fragement processing fixed functionalities.
//
/////////////////////////////////////////////////////////////////////////////
GLuint makeShaderProgramFromFiles( const char *vertShaderSrcFilename, 
								   const char *fragShaderSrcFilename,
						           void (*bindAttribLocFunc)( GLuint progObj ) )
{
	// Read shaders' source files.

	GLchar *vertSrc = NULL, *fragSrc = NULL;

	if ( vertShaderSrcFilename != NULL )
		if ( readShaderSource( vertShaderSrcFilename, &vertSrc ) == 0 )
		{
			free( vertSrc ); free( fragSrc );
			return 0;
		}

	if ( fragShaderSrcFilename != NULL )
		if ( readShaderSource( fragShaderSrcFilename, &fragSrc ) == 0 )
		{
			free( vertSrc ); free( fragSrc );
			return 0;
		}

	// Create shader program object.
	GLuint shaderProg = makeShaderProgram( vertSrc, fragSrc, bindAttribLocFunc);

	free( vertSrc ); free( fragSrc );
	return shaderProg;
}

