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
	 NetworkSession( Renderer *currentRenderer = nullptr );
	~NetworkSession();

public:
	// My Socket
	UDPSocket *m_mySocket = nullptr;

	// Connections - clients or host
	NetworkConnections			 m_connections = { nullptr };	// Vector of all the connections
	NetworkMessageDefinitionsMap m_registeredMessages;			// Map of < name, NetworkMessageInfo >

public:
	// UI
	Renderer*					m_theRenderer		= nullptr;
	Camera*						m_uiCamera			= nullptr;
	BitmapFont*					m_fonts				= nullptr;
	Vector2 const				m_screenBottomLeft	= Vector2( -g_aspectRatio, -1.f );
	Vector2 const				m_screenTopRight	= Vector2(  g_aspectRatio,  1.f );
	AABB2	const				m_screenBounds		= AABB2  ( m_screenBottomLeft, m_screenTopRight );
	Rgba	const				m_uiBackgroundColor = Rgba   ( 0, 0, 0, 100 );
	float	const				m_uiTitleFontSize	= 0.03f;
	float	const				m_uiBodyFontSize	= 0.025f;

public:
	void Render() const;

public:
	// Network Operations
	bool BindPort( uint16_t port, uint16_t range );

	void ProcessIncoming();
	void ProcessOutgoing();

	void SendPacket( NetworkPacket &packetToSend );		// Replaces connectionIndex by sender's index
	void SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address );

	uint8_t GetMyConnectionIndex() const;				// Returns 0xff if not found

public:
	// Connections & Messages
	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );	// Adds a new client
	NetworkConnection* GetConnection( int idx );

	void RegisterNetworkMessage( char const *messageName, networkMessage_cb cb );
};
