#pragma once
#include "NetworkPacket.hpp"

NetworkPacket::NetworkPacket()
	: BytePacker( PACKET_MTU, LITTLE_ENDIAN )
{

}

NetworkPacket::NetworkPacket( uint8_t connectionIdx )
	: BytePacker( PACKET_MTU, LITTLE_ENDIAN )
	, m_header( connectionIdx )
{

}

NetworkPacket::~NetworkPacket()
{

}

void NetworkPacket::WriteHeader( NetworkPacketHeader const &header )
{
	// Write header according to individual data variables
	BytePacker networkHeaderBP( NETWORK_PACKET_HEADER_SIZE, LITTLE_ENDIAN );
	networkHeaderBP.WriteBytes( sizeof( header.connectionIndex ),		&header.connectionIndex );
	networkHeaderBP.WriteBytes( sizeof( header.ack ),					&header.ack );
	networkHeaderBP.WriteBytes( sizeof( header.highestReceivedAck ),	&header.highestReceivedAck );
	networkHeaderBP.WriteBytes( sizeof( header.receivedAcksHistory ),	&header.receivedAcksHistory );
	networkHeaderBP.WriteBytes( sizeof( header.messageCount ),			&header.messageCount);
	
	// Rewrite it to buffer
	if( GetWrittenByteCount() == 0 )
		WriteBytes( networkHeaderBP.GetWrittenByteCount(), networkHeaderBP.GetBuffer(), false );
	else
		memcpy( m_buffer, networkHeaderBP.GetBuffer(), networkHeaderBP.GetWrittenByteCount() );
}

bool NetworkPacket::ReadHeader( NetworkPacketHeader &outHeader ) const
{
	// To start reading from the beginning
	ResetRead();

	size_t totalReadBytes = 0;
	totalReadBytes += ReadBytes( &outHeader.connectionIndex,		sizeof( outHeader.connectionIndex ) );
	totalReadBytes += ReadBytes( &outHeader.ack,					sizeof( outHeader.ack ) );
	totalReadBytes += ReadBytes( &outHeader.highestReceivedAck,		sizeof( outHeader.highestReceivedAck ) );
	totalReadBytes += ReadBytes( &outHeader.receivedAcksHistory,	sizeof( outHeader.receivedAcksHistory ) );
	totalReadBytes += ReadBytes( &outHeader.messageCount,			sizeof( outHeader.messageCount ) );
	
	return (totalReadBytes == NETWORK_PACKET_HEADER_SIZE);
}

bool NetworkPacket::WriteMessage( NetworkMessage const &msg )
{
	// We don't wanna overwrite the header data..
	// If we didn't write header before, and it is the first message that we are writing
	size_t bytesWritten = GetWrittenByteCount();
	if( bytesWritten == 0U )
		WriteHeader( m_header );															// Write the header

	BytePacker messagePacker( LITTLE_ENDIAN );

	size_t messageBytes			 = msg.GetWrittenByteCount();
	size_t messagePlusHeaderSize = NETWORK_MESSAGE_HEADER_SIZE + messageBytes;
	uint16_t bytesCountToWrite	 = ((uint16_t)messagePlusHeaderSize);

	// Write bytes-to-read
	messagePacker.WriteBytes( 2U, &bytesCountToWrite );

	// Write message header - all the variables, one by one
	messagePacker.WriteBytes( sizeof(msg.m_header.networkMessageDefinitionIndex), &msg.m_header.networkMessageDefinitionIndex );

	// Write message
	messagePacker.WriteBytes( messageBytes, msg.GetBuffer(), false );						// false because it is already in LITTLE_ENDIANESS

	// See if the length of messagePacker's buffer is not too big!
	size_t writableBytes	= GetWritableByteCount();
	size_t needTotalBytes	= messagePacker.GetWrittenByteCount();
	if( writableBytes < needTotalBytes )
		return false;
	else
	{
		// Write Message
		bool writeSuccess = WriteBytes( needTotalBytes, messagePacker.GetBuffer(), false );	// false because it is already in LITTLE_ENDIANESS
		GUARANTEE_RECOVERABLE( writeSuccess, "Error: Couldn't write to Network Packet!" );
		
		// Update header for new unreliable message count
		m_header.messageCount++;
		WriteHeader( m_header );

		return true;
	}
}

bool NetworkPacket::ReadMessage( NetworkMessage &outMessage ) const
{
	// Get Length of Message & Header
	uint16_t messageAndHeaderLength;
	size_t	 sizeBytes = ReadBytes( &messageAndHeaderLength, 2U );
	if( sizeBytes != 2U )
		return false;
	
	// Get Header
	size_t messageHeaderSize = NETWORK_MESSAGE_HEADER_SIZE;
	size_t headerBytes		 = ReadBytes( &outMessage.m_header, messageHeaderSize );
	if( headerBytes != messageHeaderSize )
		return false;

	// Get Message
	uint16_t messageLength		= messageAndHeaderLength - (uint16_t)messageHeaderSize;
	void	*messageBuffer		= malloc( messageLength );
	size_t	 readMessageBytes	= ReadBytes( messageBuffer, messageLength, false );						// false because we want to read it as LITTLE_ENDIAN
	
	// Out Message
	outMessage.ResetWrite();
	bool writeSuccess			= outMessage.WriteBytes( readMessageBytes, messageBuffer, false );		// false because we want to write it as LITTLE_ENDIAN
	free( messageBuffer );
	if( writeSuccess == false )
		return false;

	return true;
}

bool NetworkPacket::IsValid() const
{
	// Before return, we'll set readHead back to its original position
	size_t preservedReadHead = m_readHead;

	// Start from beginning
	ResetRead();

	// Read PacketHeader
	NetworkPacketHeader packetHeader;
	bool readSuccess = ReadHeader( packetHeader );
	if( readSuccess == false )
	{
		m_readHead = preservedReadHead;
		return false;
	}

	// Fetch Message Count
	uint8_t	messageCount = packetHeader.messageCount;

	// Skip each messages..
	while ( messageCount > 0 )
	{
		// Read length of bytes in this Packed Message
		uint16_t messageAndHeaderBytes = 0x00;
		size_t	 lengthBytes = ReadBytes( &messageAndHeaderBytes, 2U );
		if( lengthBytes != 2U )
		{
			m_readHead = preservedReadHead;
			return false;
		}

		// Skip this whole message..
		bool moveSuccessful = MoveReadheadBy( (double)messageAndHeaderBytes );
		if( moveSuccessful == false )
		{
			m_readHead = preservedReadHead;
			return false;
		}

		messageCount--;
	}

	// Bytes left should be ZERO; b/c we should be reached to the end
	size_t bytesLeftInBuffer = m_writeHead - m_readHead;

	m_readHead = preservedReadHead;
	return (bytesLeftInBuffer == 0U);
}

bool NetworkPacket::HasMessages() const
{
	return m_header.messageCount > 0U;
}
