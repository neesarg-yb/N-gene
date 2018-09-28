#pragma once
#include "Engine/Network/BytePacker.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

struct PacketHeader
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
	PacketHeader m_header;

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
	void WriteHeader( PacketHeader const &header );
	bool ReadHeader ( PacketHeader &outHeader );

	bool WriteMessage( NetworkMessage const &msg, int idx = -1 );
	bool ReadMessage ( NetworkMessage &outMessage );

	bool IsValid() const;
};
