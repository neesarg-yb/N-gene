#pragma once
#include "NetworkMessage.hpp"
#include "Game/GameCommon.hpp"

NetworkMessage::NetworkMessage( char const *msgName )
	: m_name( msgName )
{

}

NetworkMessage::~NetworkMessage()
{

}

bool NetworkMessage::Read( float &outFloat ) const
{
	UNUSED( outFloat );
	return false;
}

bool NetworkMessage::Read( std::string &outString ) const
{
	UNUSED( outString );
	return false;
}

void NetworkMessage::Write( float number )
{
	UNUSED( number );
}

void NetworkMessage::Write( std::string &message )
{
	UNUSED( message );
}

bool NetworkMessage::GetNextChunkOfSize( size_t chunkMaxSize, void *&outBuffer, size_t &outByteSize )
{
	UNUSED( chunkMaxSize );
	UNUSED( outBuffer );
	UNUSED( outByteSize );

	// Outs a chunk => [ chunkNumber | isLastChunk | partOfTheMessage ], of size chunkMaxSize..

	return false;
}

bool NetworkMessage::GetChunkAtIndex( size_t idx, size_t chunkMaxSize, void *&outBuffer, size_t &outBytes )
{
	UNUSED( idx );
	UNUSED( chunkMaxSize );
	UNUSED( outBuffer );
	UNUSED( outBytes );

	return false;
}

void NetworkMessage::WriteChunkAtIndex( void *chunkBuffer, size_t bytesToWrite, size_t idx )
{
	UNUSED( idx );
	UNUSED( chunkBuffer );
	UNUSED( bytesToWrite );
}
