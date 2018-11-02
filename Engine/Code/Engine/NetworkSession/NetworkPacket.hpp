#pragma once
#include <vector>
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

#define MAX_RELIABLES_PER_PACKET (32)

class NetworkPacket;
typedef std::vector< NetworkPacket* > NetworkPacketList;

struct PacketTracker
{
public:
	uint16_t ack			= INVALID_PACKET_ACK;
	uint64_t sentTimeHPC	= Clock::GetCurrentHPC();
	uint16_t sentReliables[ MAX_RELIABLES_PER_PACKET ] = { 0U };

public:
	PacketTracker() { }
	PacketTracker( uint16_t inAck );		// Sets sentTimeHPC as current time
	PacketTracker( uint16_t inAck, uint64_t inSentTimeHPC );

public:
	bool AddNewReliableID( uint16_t reliableID );
	void TrackForAck( uint16_t inAck );

	void Invalidate();
	bool IsValid() const;
};

struct NetworkPacketHeader
{
	// Note!
	//		Don't forget to update "NETWORK_PACKET_HEADER_SIZE" in Engine/Core/EngineCommon.h
public:
	uint8_t connectionIndex = 0xff;		// When creating a NetworkPacket Idx = receiver's; When NetworkSession sends this packet, it gets replaced by sender's idx

	// Acknowledgment
	uint16_t ack				 = INVALID_PACKET_ACK;
	uint16_t highestReceivedAck	 = INVALID_PACKET_ACK;
	uint16_t receivedAcksHistory = 0U; // Bit field

	uint8_t messageCount = 0x00;

public:
	NetworkPacketHeader() { }
	NetworkPacketHeader( uint8_t connectionIdx );
	NetworkPacketHeader( uint8_t connectionIdx, uint8_t msgCount );
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
//	|-----------------------------------------------------------------------------------
//	| 2 bytes: Total Size after these two bytes | 3 bytes: *(Message Header) | Message |
//	|-----------------------------------------------------------------------------------
//

public:
	void WriteHeader( NetworkPacketHeader const &header );
	bool ReadHeader ( NetworkPacketHeader &outHeader ) const;

	bool WriteMessage( NetworkMessage const &msg );											// Writes the message and updates the header
	bool ReadMessage ( NetworkMessage &outMessage, NetworkSession const &session ) const;	// Fills the m_header for you!

	bool IsValid() const;									// Read Head doesn't get affected after this operation
	bool HasMessages() const;								// If it has at least one message
};
