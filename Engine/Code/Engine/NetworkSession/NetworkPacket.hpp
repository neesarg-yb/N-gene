#pragma once
#include <vector>
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkPacket;
typedef std::vector< NetworkPacket* > NetworkPacketList;

struct PacketTracker
{
public:
	uint16_t ack			= INVALID_PACKET_ACK;
	uint64_t sentTimeHPC	= Clock::GetCurrentHPC();

public:
	PacketTracker() { }

	PacketTracker( uint16_t inAck )		// Sets sentTimeHPC as current time
		: ack( inAck ) { }

	PacketTracker( uint16_t inAck, uint64_t inSentTimeHPC )
		: ack( inAck )
		, sentTimeHPC( inSentTimeHPC ) { }

public:
	void TrackForAck( uint16_t inAck )
	{
		ack			= inAck;
		sentTimeHPC	= Clock::GetCurrentHPC();
	}
	void Invalidate()
	{
		ack = INVALID_PACKET_ACK;
	}
};

struct NetworkPacketHeader
{
	// Note!
	//		Don't forget to update "NETWORK_PACKET_HEADER_SIZE" in Engine/Core/EngineCommon.h
public:
	uint8_t connectionIndex	= 0xff;		// When creating a NetworkPacket Idx = receiver's; When NetworkSession sends this packet, it gets replaced by sender's idx

	// Acknowledgment
	uint16_t ack							= INVALID_PACKET_ACK;
	uint16_t highestReceivedAck				= INVALID_PACKET_ACK;
	uint16_t receivedAcksHistory			= 0U; // Bit field

	uint8_t messageCount	= 0x00;

public:
	NetworkPacketHeader() { }
	NetworkPacketHeader( uint8_t connectionIdx )
	{
		connectionIndex = connectionIdx;
	}
	NetworkPacketHeader( uint8_t connectionIdx, uint8_t msgCount )
	{
		connectionIndex	 = connectionIdx;
		messageCount	 = msgCount;
	}
};

class NetworkPacket : public BytePacker
{
public:
	 NetworkPacket();
	 NetworkPacket( uint8_t connectionIdx );
	~NetworkPacket();

public:
	NetworkPacketHeader m_header;

//	Buffer:
//	                                                                                                  total size <= MTU
//	|----------------------------------------------------------------------------------------------------------------------
//	| PacketHeader: ( 1 byte: SenderIdx  | 1 byte: UnreliableMessageCount | .. ) | PackedMessage_1 | PackedMessage_2.. | ..
//	|----------------------------------------------------------------------------------------------------------------------
//
//  Packed Message:
//	|----------------------------------------------------------------------------------
//	| 2 bytes: Total Size after these two bytes | 1 byte: *(Message Header) | Message |
//	|----------------------------------------------------------------------------------
//

public:
	void WriteHeader( NetworkPacketHeader const &header );
	bool ReadHeader ( NetworkPacketHeader &outHeader ) const;

	bool WriteMessage( NetworkMessage const &msg );			// Writes the message and updates the header
	bool ReadMessage ( NetworkMessage &outMessage ) const;	// Fills the m_header for you!

	bool IsValid() const;									// Read Head doesn't get affected after this operation
	bool HasMessages() const;								// If it has at least one message
};
