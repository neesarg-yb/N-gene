#pragma once
#include "MapFileBuffer.hpp"
#include "Engine/Math/MathUtil.hpp"

MapFileBuffer::MapFileBuffer()
{

}

MapFileBuffer::~MapFileBuffer()
{
	if( m_buffer != nullptr )
	{
		free( m_buffer );
		m_buffer = nullptr;
	}
}

bool MapFileBuffer::LoadFromFile( char const *fileName )
{
	// Clean old buffer
	if( m_buffer != nullptr )
	{
		free( m_buffer );
		m_buffer = nullptr;
	}

	// Read the file into buffer
	m_buffer = (char*) FileReadToNewBuffer( fileName );
	m_readIndex = 0;

	// Failure?
	if( m_buffer == nullptr )
		return false;
	else
		return true;
}

void MapFileBuffer::SkipLeadingWhiteSpaces()
{
	// While it's a whitespace
	while( isspace( m_buffer[m_readIndex] ) != 0 )
	{
		// Skip
		m_readIndex++;
	}
}

bool MapFileBuffer::SeekNext( char sChar )
{
	uint seekIndex = m_readIndex;
	while( m_buffer[seekIndex] != sChar )
	{
		// Break if reached to the end
		if( IsEndOfBufferAt(seekIndex) )
			break;

		// Loop until we find sChar
		seekIndex++;
	}

	if( m_buffer[seekIndex] == sChar )
	{
		// Put us right on the found character
		m_readIndex = seekIndex;
		return true;
	}
	else
	{
		// We don't change the read index, if failed!
		return false;
	}
}

char MapFileBuffer::ReadNextCharacter()
{
	// If we're on end of buffer
	if( IsEndOfBufferAt(m_readIndex) )
		return EOF;

	char toReturn = m_buffer[m_readIndex];
	m_readIndex++;

	return toReturn;
}

char MapFileBuffer::PeekNextCharacter() const
{
	if( IsEndOfBufferAt(m_readIndex) )
		return EOF;

	if( m_readIndex != 0 )
	{
		// If we were on end of buffer, consider the next character as EOF..
		uint lastCharIndex = m_readIndex - 1;
		if( IsEndOfBufferAt(lastCharIndex) )
			return EOF;
	}

	return m_buffer[m_readIndex];
}

bool MapFileBuffer::ReadNextString( std::string &str_out, char const wrappedBy /*= EOF */ )
{
	if( m_readIndex != 0 )
	{
		// If we were already at end of the buffer..
		if( IsEndOfBufferAt(m_readIndex-1) )
			return false;
	}

	str_out = "";
	if( wrappedBy == EOF )
	{
		// Read until we encounter a whitespace
		while( isspace( m_buffer[m_readIndex] ) == 0 )
		{
			if( IsEndOfBufferAt(m_readIndex) )
				return true;

			// Add the character to string
			str_out += m_buffer[m_readIndex];

			// To read next one..
			m_readIndex++;
		}
	}
	else
	{
		// Staring by the wrapper character.. Skip it
		char wrapperIndicator = ReadNextCharacter();
		if( wrapperIndicator != wrappedBy )
			return false;

		// Read until we encounter tailing wrapper character
		while( m_buffer[m_readIndex] != wrappedBy )
		{
			if( IsEndOfBufferAt(m_readIndex) )
				return false;

			// Add the character to string
			str_out += m_buffer[m_readIndex];

			// To read next one..
			m_readIndex++;
		}

		// Skip the tailing wrapper character
		if( m_buffer[m_readIndex] == wrappedBy )
			m_readIndex++;
	}

	return (str_out != "");
}

bool MapFileBuffer::ReadNextFloat( float &float_out )
{
	std::string floatStr;
	bool readSuccess = ReadNextString( floatStr );
	
	if( readSuccess == false )
		return false;

	::SetFromText( float_out, floatStr.c_str() );
	return true;
}

bool MapFileBuffer::ReadNextVector3( Vector3 &vec3_out )
{
	// ( 
	char indicator = ReadNextCharacter();
	if( indicator != '(' )
		return false;

	// x y z
	SkipLeadingWhiteSpaces();
	bool xReadSuccess = ReadNextFloat( vec3_out.x );
	SkipLeadingWhiteSpaces();
	bool yReadSuccess = ReadNextFloat( vec3_out.y );
	SkipLeadingWhiteSpaces();
	bool zReadSuccess = ReadNextFloat( vec3_out.z );

	// )
	SkipLeadingWhiteSpaces();
	indicator = ReadNextCharacter();

	return indicator && (xReadSuccess && yReadSuccess && zReadSuccess);
}

bool MapFileBuffer::IsEndOfBufferAt( uint readAt ) const
{
	// If buffer is empty
	if( m_buffer == nullptr )
		return true;

	char currentChar = m_buffer[ readAt ];
	// If at end of file
	if( currentChar == EOF )
		return true;
	// If at null terminator
	if( currentChar == '\0' )
		return true;

	// Not end of the buffer
	return false;
}
