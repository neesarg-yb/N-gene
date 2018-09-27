#pragma once#
#include <vector>
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkSession;

typedef std::vector< NetworkMessage* > NetworkMessages;

class NetworkConnection
{
public:
	 NetworkConnection( int idx, NetworkAddress &addr, NetworkSession &parentSession );
	~NetworkConnection();

public:
	NetworkAddress		 m_address;
	NetworkSession		&m_parentSession;
	int					 m_indexInSession;

	NetworkMessages		 m_outgoingMessages;		// Unreliable messages, for now

public:
	void ProcessOutgoing();
};
