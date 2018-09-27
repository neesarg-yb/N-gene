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
	PacketHeader m_header;

public:
	void WriteHeader( PacketHeader const &header );
	bool ReadHeader( PacketHeader const &header );

	bool WriteMessage( NetworkMessage const &msg );
	bool ReadMessage( NetworkMessage &outMessage );
};
