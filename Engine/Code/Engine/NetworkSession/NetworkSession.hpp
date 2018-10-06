#pragma once
#include <map>
#include <vector>
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkConnection.hpp"

#define MAX_SESSION_CONNECTIONS (0xff)

// Session Message Callbacks
typedef NetworkConnection*		NetworkConnections[ MAX_SESSION_CONNECTIONS ];
typedef std::map< std::string,	NetworkMessageDefinition >	NetworkMessageDefinitionsMap;

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
	NetworkConnections			 m_connections = { nullptr };	// Vector of all the connections
	NetworkMessageDefinitionsMap m_registeredMessages;			// Map of < name, NetworkMessageInfo >

public:
	bool BindPort( uint16_t port, uint16_t range );

	void ProcessIncoming();
	void ProcessOutgoing();

	void SendPacket( NetworkPacket &packetToSend );		// Replaces connectionIndex by sender's index
	void SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address );

	uint8_t GetMyConnectionIndex() const;				// Returns 0xff if not found

public:
	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );	// Adds a new client
	NetworkConnection* GetConnection( int idx );

	void RegisterNetworkMessage( char const *messageName, networkMessage_cb cb );
};
