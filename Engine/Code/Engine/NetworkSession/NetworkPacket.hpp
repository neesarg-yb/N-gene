#pragma once
#include <vector>
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkPacket;
typedef std::vector< NetworkPacket* > NetworkPacketList;

struct NetworkPacketHeader
{
public:
	uint8_t connectionIndex			= 0xff;		// When creating a NetworkPacket Idx = receiver's; When NetworkSession sends this packet, it gets replaced by sender's idx
	uint8_t unreliableMessageCount	= 0x00;

public:
	NetworkPacketHeader() { }
	NetworkPacketHeader( uint8_t connectionIdx )
	{
		connectionIndex = connectionIdx;
	}
	NetworkPacketHeader( uint8_t connectionIdx, uint8_t unreliableMsgCount )
	{
		connectionIndex  = connectionIdx;
		unreliableMessageCount = unreliableMsgCount;
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
//	                                                                                total size <= MTU
//	|------------------------------------------------------------------------------------------------
//	| 1 byte: SenderIdx  | 1 byte: UnreliableMessageCount | PackedMessage_1 | PackedMessage_2.. | ..
//	|------------------------------------------------------------------------------------------------
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
};
