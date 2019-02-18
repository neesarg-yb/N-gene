#pragma once
#include "MapFileBuffer.hpp"

MapFileBuffer::MapFileBuffer()
{

}

MapFileBuffer::~MapFileBuffer()
{

}

bool MapFileBuffer::LoadFromFile( char const *fileName )
{
	UNUSED( fileName );
	return false;
}

void MapFileBuffer::SkipLeadingWhiteSpaces()
{

}

bool MapFileBuffer::SeekNext( char sChar )
{
	UNUSED( sChar );
	return false;
}

char MapFileBuffer::ReadNextCharacter()
{
	return EOF;
}

char MapFileBuffer::PeekNextCharacter() const
{
	return EOF;
}

bool MapFileBuffer::ReadNextString( std::string &str_out, char const wrappedBy /*= EOF */ )
{
	UNUSED( str_out );
	UNUSED( wrappedBy );
	return false;
}

bool MapFileBuffer::ReadNextFloat( float &float_out )
{
	UNUSED( float_out );
	return false;
}

bool MapFileBuffer::ReadNextVector3( Vector3 &vec3_out )
{
	UNUSED( vec3_out );
	return false;
}
