#pragma once
#include <string>
#include "Engine/Network/BytePacker.hpp"

class NetworkMessage
{
public:
	NetworkMessage( char const *msgName );
	~NetworkMessage();

public:
	std::string const  m_name;
	BytePacker		  *m_bytePacker = nullptr;

public:
	bool Read( float &outFloat ) const;
	bool Read( std::string &outString ) const;

	void Write( float number );
	void Write( std::string &message );

	bool GetNextChunkOfSize	( size_t chunkMaxSize, void *&outBuffer, size_t &outByteSize );
	bool GetChunkAtIndex	( size_t idx, size_t chunkMaxSize, void *&outBuffer, size_t &outBytes );

	void WriteChunkAtIndex	( void *chunkBuffer, size_t bytesToWrite, size_t idx );
};