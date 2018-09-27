#pragma once
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkSession;

struct NetworkConnection
{
public:
	NetworkConnection( NetworkAddress &addr, NetworkSession &parentSession );
	~NetworkConnection();

public:
	NetworkAddress  connection;
	NetworkSession &session;

public:
	void Send( NetworkMessage &msgToSend );
};

struct MessageQueueElement
{
public:
	MessageQueueElement( NetworkAddress &connectionAddr, NetworkMessage &msg );

	NetworkAddress	connection;
	NetworkMessage	message;
	bool			operationComplete = false;		// Can be used to denote "Sent one time", or "Received the full message"
};