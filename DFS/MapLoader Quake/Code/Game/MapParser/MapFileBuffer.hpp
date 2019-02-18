#pragma once
#include "Engine/File/File.hpp"
#include "Engine/Math/Vector3.hpp"

class MapFileBuffer
{
public:
	 MapFileBuffer();
	~MapFileBuffer();

private:
	char *m_buffer		= nullptr;
	uint  m_readIndex	= 0U;

public:
	bool LoadFromFile( char const *fileName );

public:
	// Fast forward
	void SkipLeadingWhiteSpaces();
	bool SeekNext( char sChar );												// Puts you right on the next sChar in the buffer!

	// Reading operations
	char ReadNextCharacter();													// Returns EOF, if nothing left to read
	bool ReadNextString	( std::string &str_out, char const wrappedBy = EOF );	// Wrapped by EOF means, it is not wrapped by any character..
	bool ReadNextFloat	( float &float_out );
	bool ReadNextVector3( Vector3 &vec3_out );

	// Without reading
	char PeekNextCharacter() const;												// Returns EOF, if nothing left to peek

private:
	bool IsEndOfBufferAt( uint readAt ) const;									// Returns true if can't read further
};
