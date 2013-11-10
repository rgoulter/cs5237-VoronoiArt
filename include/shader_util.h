#ifndef _SHADER_UTIL_H_
#define _SHADER_UTIL_H_

/////////////////////////////////////////////////////////////////////////////
// Print OpenGL error if there is any.
// Returns 1 if an OpenGL error occurred, 0 otherwise.
/////////////////////////////////////////////////////////////////////////////
#define printOpenGLError()  printOglError( __FILE__, __LINE__ )


/////////////////////////////////////////////////////////////////////////////
// Print OpenGL error if there is any.
// Returns 1 if an OpenGL error occurred, 0 otherwise.
/////////////////////////////////////////////////////////////////////////////
extern int printOglError( char *file, int line );


/////////////////////////////////////////////////////////////////////////////
// Reads shader source code from file fileName. 
// It allocates memory for the null-terminated text string, 
// pointed to by *shaderSource.
// Returns 0 on error, otherwise return 1.
/////////////////////////////////////////////////////////////////////////////
extern int readShaderSource( const char *fileName, GLchar **shaderSource );


/////////////////////////////////////////////////////////////////////////////
// Get the location of a uniform variable.
/////////////////////////////////////////////////////////////////////////////
extern GLint getUniLoc( GLuint program, const GLchar *name );


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
extern GLuint makeShaderProgram( const GLchar *vertShaderSrcStr, 
						         const GLchar *fragShaderSrcStr,
						         void (*bindAttribLocFunc)( GLuint progObj ) );



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
extern GLuint makeShaderProgramFromFiles( const char *vertShaderSrcFilename, 
								          const char *fragShaderSrcFilename,
						                  void (*bindAttribLocFunc)( GLuint progObj ) );


#endif
