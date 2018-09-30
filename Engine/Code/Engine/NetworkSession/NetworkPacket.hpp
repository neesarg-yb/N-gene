#pragma once
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

struct NetworkPacketHeader
{
	uint8_t senderConnectionIndex;
	uint8_t unreliableMessageCount;
};

class NetworkPacket : public BytePacker
{
public:
	 NetworkPacket();
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

	bool IsValid() const;
};
