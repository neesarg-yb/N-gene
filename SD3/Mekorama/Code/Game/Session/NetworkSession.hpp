#pragma once
#include "Engine/Network/NetworkAddress.hpp"

class NetworkMessage;

struct NetworkSender
{
public:
	NetworkAddress const connection;

public:
	bool Send( NetworkMessage &msgToSend ) const;
};

typedef bool (*sessionMessage_cb) ( NetworkMessage const &, NetworkSender const & );

class NetworkSession
{
public:
	 NetworkSession();
	~NetworkSession();

public:
	void AddBinding( uint16_t port );
	void RegisterMessage( char const *messageName, sessionMessage_cb cb );

	void ProcessIncoming();
	void ProcessOutgoing();
};