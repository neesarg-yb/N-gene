#pragma once
#include "Engine/Network/NetworkAddress.hpp"

class NetworkMessage;

struct NetworkConnection
{
public:
	NetworkAddress const connection;

public:
	bool Send( NetworkMessage &msgToSend ) const;
};

typedef bool (*sessionMessage_cb) ( NetworkMessage const &, NetworkConnection const & );

class NetworkSession
{
public:
	 NetworkSession();
	~NetworkSession();

public:
	void AddBinding( uint16_t port );
	void RegisterMessage( char const *messageName, sessionMessage_cb cb );

	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );
	NetworkConnection* GetConnection( int idx );

	void ProcessIncoming();
	void ProcessOutgoing();
};