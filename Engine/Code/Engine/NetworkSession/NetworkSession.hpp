#pragma once
#include <map>
#include <vector>
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkConnection.hpp"

// Session Message Callbacks
typedef std::vector< NetworkConnection* >					NetworkConnections;
typedef std::map< std::string, NetworkMessageDefinition >	NetworkMessageDefinitionsMap;

class NetworkPacket;

class NetworkSession
{
public:
	 NetworkSession();
	~NetworkSession();

public:
	// My Socket
	UDPSocket *m_mySocket = nullptr;

	// Connections - clients or host
	NetworkConnections			 m_connections;			// Vector of all the connections
	NetworkMessageDefinitionsMap m_registeredMessages;	// Map of < name, NetworkMessageInfo >

public:
	bool BindPort( uint16_t port, uint16_t range );

	void ProcessIncoming();
	void ProcessOutgoing();

	void SendPacket( NetworkPacket const &packetToSend );
	void SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address );

public:
	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );	// Adds a new client
	NetworkConnection* GetConnection( int idx );

	void RegisterNetworkMessage( char const *messageName, networkMessage_cb cb );
};
