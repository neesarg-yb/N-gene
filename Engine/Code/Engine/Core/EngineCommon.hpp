#pragma once
#include <assert.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/../ThirdParty/tinyxml/tinyxml2.h"

typedef unsigned char byte_t;
typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;

const  float	   g_aspectRatio = 1.77f;

enum eTextureFormat 
{
	TEXTURE_FORMAT_RGBA8,	// default color format
	TEXTURE_FORMAT_D24S8, 
}; 

enum ePrimitiveType
{
	PRIMITIVE_POINTS,		// in OpenGL, for example, this becomes GL_POINTS
	PRIMITIVE_LINES,		// in OpenGL, for example, this becomes GL_LINES
	PRIMITIVE_TRIANGES,		// in OpenGL, for example, this becomes GL_TRIANGLES
	NUM_PRIMITIVE_TYPES
};

#define UNUSED(x) (void)(x);
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

// Source from http://www.flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " --------------------------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " --------------------------------------------------------------------------------------\n" )
#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" ) GUARANTEE_RECOVERABLE( false, "Interesting..!");
#define GL_BIND_FUNCTION(f)      wglGetTypedProcAddress( &f, #f )


// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render contect
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( gGLLibrary, name); 
	}

	return (*out != nullptr); 
}