#pragma once
#include <assert.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"

class Clock;

typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;
typedef unsigned int	uint;
typedef unsigned char	byte_t;

#define MAX_LIGHTS 8U
const   float	   g_aspectRatio = 1.77f;

#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

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


// NETWORK SESSION
#define MAX_SESSION_CONNECTIONS					(0xff)
#define INVALID_PACKET_ACK						(0xffff)
#define MAX_TRACKED_PACKETS						(32)
#define NETWORK_PACKET_HEADER_SIZE				(8)
#define NETWORK_UNRELIABLE_MESSAGE_HEADER_SIZE	(1)
#define NETWORK_RELIABLE_MESSAGE_HEADER_SIZE	(3)

constexpr uint16_t RELIABLE_MESSAGES_WINDOW = 8;


// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render contact
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( gGLLibrary, name); 
	}

	return (*out != nullptr); 
}

void EngineStartup();
void EngineShutdown();

Clock const*	GetMasterClock();
void			TickMasterClock();		// Advances the master clock by a frame
