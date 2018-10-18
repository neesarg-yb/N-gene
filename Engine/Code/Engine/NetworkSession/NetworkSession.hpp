#pragma once
#include <queue>
#include <vector>
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkConnection.hpp"

#define MAX_SESSION_CONNECTIONS (0xff)

class  NetworkPacket;
struct StampedNetworkPacket;
struct CustomCompareForStampedPacketQueue;

// Session Message Callbacks
typedef NetworkConnection*																									NetworkConnections[ MAX_SESSION_CONNECTIONS ];
typedef std::map< std::string,	NetworkMessageDefinition >																	NetworkMessageDefinitionsMap;
typedef std::priority_queue< StampedNetworkPacket, std::vector<StampedNetworkPacket>, CustomCompareForStampedPacketQueue >	StampedNetworkPacketPriorityQueue;

struct StampedNetworkPacket
{
public:
	NetworkPacket	*packet;
	NetworkAddress	 sender;
	uint64_t		 timestampHPC;

public:
	StampedNetworkPacket( NetworkPacket *netPacket, NetworkAddress &senderAddr )
		: sender( senderAddr )
		, packet( netPacket )
		, timestampHPC( 0U ) { }
};

// Puts the lowest timestamp having StampedPacket at the top
struct CustomCompareForStampedPacketQueue
{
	bool operator () ( StampedNetworkPacket &lhs, StampedNetworkPacket &rhs )
	{
		return lhs.timestampHPC > rhs.timestampHPC;
	}
};

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
	void RegisterCoreMessages();

	void ProcessIncoming();
	void ProcessOutgoing();

	void SendPacket( NetworkPacket &packetToSend );		// Replaces connectionIndex by sender's index
	void SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address );

	uint8_t				GetMyConnectionIndex() const;				// Returns 0xff if not found
	NetworkConnection*	GetMyConnection();							// Will crashes if not found

public:
	// Connections & Messages
	NetworkConnection* AddConnection( int idx, NetworkAddress &addr );	// Adds a new client
	NetworkConnection* GetConnection( int idx );

	void RegisterNetworkMessage( char const *messageName, networkMessage_cb cb );

private:
	// Net Simulation
	float	m_simulatedLossFraction	 = 0.f;
	uint	m_simulatedMinLatency_ms = 0U;
	uint	m_simulatedMaxLatency_ms = 0U;
	uint8_t m_simulatedSendFrequency = 20;
	float	m_heartbeatFrequency	 = 0.2f;

private:
	// Priority Queue
	StampedNetworkPacketPriorityQueue m_receivedPackets;

public:
	inline float	GetHeartbeatFrequency()		const { return m_heartbeatFrequency; }
	inline float	GetSimulatedLossFraction()	const { return m_simulatedLossFraction; }
	inline uint8_t	GetSimulatedSendFrequency()	const { return m_simulatedSendFrequency; }

	bool SetHeartbeatFrequency( float frequencyHz );
	void SetSimulationLoss( float lossFraction );
	void SetSimulationLatency( uint minAddedLatency_ms, uint maxAddedLatency_ms = 0U );
	void SetSimulationSendFrequency( uint8_t frequencyHz );

private:
	void ReceivePacket();
	void ProcessReceivedPackets();
	void QueuePacketForSimulation( NetworkPacket *newPacket, NetworkAddress &sender );	// Queues them with a random latency
	void ProccessAndDeletePacket ( NetworkPacket *&packet, NetworkAddress &sender );	// Process the packet and deletes it
};
