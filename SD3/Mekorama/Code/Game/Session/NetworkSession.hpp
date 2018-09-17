#pragma once
#include <map>
#include <vector>
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Game/Session/NetworkMessage.hpp"

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

// Session Message Callbacks
typedef bool (*sessionMessage_cb) ( NetworkMessage const &, NetworkConnection & );
typedef std::vector< sessionMessage_cb > SessionMessageCallbacks;

class NetworkSession
{
public:
	 NetworkSession();
	~NetworkSession();

public:
	// My Socket
	UDPSocket *m_mySocket = nullptr;

	// Connections - clients or host
	std::vector< NetworkConnection* > m_connections;

	// Message Queue
	std::vector< MessageQueueElement > m_outgoingMessages;
	std::vector< MessageQueueElement > m_incomingMessages;

	// Registered Message Callbacks
	std::map< std::string, SessionMessageCallbacks > m_registeredMessages;

public:
	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );	// Adds a new client
	NetworkConnection* GetConnection( int idx );

	void RegisterMessage( char const *messageName, sessionMessage_cb cb );

public:
	// Hosting, Message Handling
	bool AddBinding( uint16_t port );

	void QueueMessageToSend( NetworkMessage &msgToSend, NetworkAddress &receiver );
	void ProcessIncoming();
	void ProcessOutgoing();
};