#pragma once
#include "NetworkPacket.hpp"

NetworkPacket::NetworkPacket()
	: BytePacker( PACKET_MTU, LITTLE_ENDIAN )
{

}

NetworkPacket::~NetworkPacket()
{

}

void NetworkPacket::WriteHeader( PacketHeader const &header )
{
	uint8_t *headerOnBuffer = (uint8_t *)m_buffer;

	headerOnBuffer[0] = header.senderConnectionIndex;
	headerOnBuffer[1] = header.unreliableMessageCount;
}

bool NetworkPacket::ReadHeader( PacketHeader &outHeader )
{
	ResetRead();

	size_t expectedBytes	= sizeof( PacketHeader );
	size_t readHeaderBytes	= ReadBytes( &outHeader.senderConnectionIndex, expectedBytes );

	return (expectedBytes == readHeaderBytes);
}

bool NetworkPacket::WriteMessage( NetworkMessage const &msg, int idx /* = -1 */ )
{
	BytePacker messagePacker( LITTLE_ENDIAN );

	size_t messageBytes			 = msg.GetWrittenByteCount();
	size_t messagePlusHeaderSize = sizeof( NetworkMessageHeader ) + messageBytes;

	// Write bytes-to-read
	messagePacker.WriteBytes( 2U, (uint16_t*)&messagePlusHeaderSize );

	// Write message header
	messagePacker.WriteBytes( sizeof( NetworkMessageHeader ), &msg.m_header );

	// Write message
	messagePacker.WriteBytes( messageBytes, msg.GetBuffer(), false );		// false because it is already in LITTLE_ENDIANESS

	// See if the length of messagePacker's buffer is not too big!
	size_t writableBytes	= GetWritableByteCount();
	size_t needTotalBytes	= messagePacker.GetWrittenByteCount();
	if( writableBytes < needTotalBytes )
		return false;
	else
	{
		WriteBytes( needTotalBytes, messagePacker.GetBuffer(), false );		// false because it is already in LITTLE_ENDIANESS
		return true;
	}
}

bool NetworkPacket::ReadMessage( NetworkMessage &outMessage )
{
	// Get Length of Message & Header
	uint16_t messageAndHeaderLength;
	size_t	 sizeBytes = ReadBytes( &messageAndHeaderLength, 2U );
	if( sizeBytes != 2U )
		return false;
	
	// Get Header
	size_t messageHeaderSize = sizeof( NetworkMessageHeader );
	size_t headerBytes		 = ReadBytes( &outMessage.m_header, messageHeaderSize );
	if( headerBytes != messageHeaderSize )
		return false;	// INVALID

	// Get Message
	uint16_t messageLength		= messageAndHeaderLength - (uint16_t)messageHeaderSize;
	void	*messageBuffer		= malloc( messageLength );
	size_t	 readMessageBytes	= ReadBytes( messageBuffer, messageLength, false );						// false because we want to read it as LITTLE_ENDIAN
	
	// Out Message
	outMessage.ResetWrite();
	size_t	bytesOutAsMessage	= outMessage.WriteBytes( readMessageBytes, messageBuffer, false );		// false because we want to write it as LITTLE_ENDIAN
	free( messageBuffer );
	if( bytesOutAsMessage != readMessageBytes )
		return false;

	return true;
}

bool NetworkPacket::IsValid() const
{
	return false;
}
