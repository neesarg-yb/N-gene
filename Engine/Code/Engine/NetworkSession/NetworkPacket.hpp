#pragma once
#include <vector>
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkPacket;
typedef std::vector< NetworkPacket* > NetworkPacketList;

struct NetworkPacketHeader
{
public:
	uint8_t connectionIndex	= 0xff;		// When creating a NetworkPacket Idx = receiver's; When NetworkSession sends this packet, it gets replaced by sender's idx
	uint8_t messageCount	= 0x00;

	// Acknowledgment
	uint16_t ack;
	uint16_t lastReceivedAck;
	uint16_t previouslyReceivedAckBitfield;

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
//	|-------------------------------------------------------------------------------------------------------------------
//	| PacketHeader: ( 1 byte: SenderIdx  | 1 byte: UnreliableMessageCount | .. ) | PackedMessage_1 | PackedMessage_2.. | ..
//	|-------------------------------------------------------------------------------------------------------------------
//
//  Packed Message:
//	|----------------------------------------------------------------------------------
//	| 2 bytes: Total Size after these two bytes | 1 byte: *(Message Header) | Message |
//	|----------------------------------------------------------------------------------
//

public:
	void WriteHeader( NetworkPacketHeader const &header );
	bool ReadHeader ( NetworkPacketHeader &outHeader );

	bool WriteMessage( NetworkMessage const &msg );			// Writes the message and updates the header
	bool ReadMessage ( NetworkMessage &outMessage );		// Fills the m_header for you!

	bool IsValid() const;									// Read Head doesn't get affected after this operation
	bool HasMessages() const;								// If it has at least one message
};
