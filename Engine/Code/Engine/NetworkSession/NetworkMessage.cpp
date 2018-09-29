#pragma once
#include "NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

NetworkMessage::NetworkMessage( std::string &name )
	: BytePacker( PACKET_MTU - sizeof(PacketHeader) - 2U , LITTLE_ENDIAN )
	, m_name( name )
{

}

NetworkMessage::~NetworkMessage()
{

}

bool NetworkMessage::Write( int number )
{
	return WriteBytes( sizeof(int), &number );
}

bool NetworkMessage::Write( float number )
{
	return WriteBytes( sizeof(float), &number );
}

bool NetworkMessage::Write( std::string const &string )
{
	return WriteString( string.c_str() );
}

bool NetworkMessage::Raad( int &outNumber ) const
{
	size_t expectedBytes = sizeof( int );
	size_t bytesRead	 = ReadBytes( &outNumber, expectedBytes );

	return (expectedBytes == bytesRead);
}

bool NetworkMessage::Read( float &outNumber ) const
{
	size_t expectedBytes = sizeof( float );
	size_t bytesRead	 = ReadBytes( &outNumber, expectedBytes );

	return (expectedBytes == bytesRead);
}

size_t NetworkMessage::Read( char *outString, size_t maxSize ) const
{
	return ReadString( outString, maxSize );
}
