#pragma once
#include <queue>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkConnection.hpp"


//---------------------
// Forward Declarations
//
class  NetworkPacket;
struct StampedNetworkPacket;
struct CustomCompareForStampedPacketQueue;

// Session Message Callbacks
typedef NetworkConnection*																									NetworkConnectionsArray[ MAX_SESSION_CONNECTIONS ];
typedef std::vector< NetworkConnection* >																					NetworkConnectionsVector;
typedef std::priority_queue< StampedNetworkPacket, std::vector<StampedNetworkPacket>, CustomCompareForStampedPacketQueue >	StampedNetworkPacketPriorityQueue;


//----------
// Constants
//
constexpr uint16_t DEFAULT_PORT_RANGE = 10;


//------
// Enum
//
enum eNetworkSessionState
{
	NET_SESSION_DISCONNECTED = 0,
	NET_SESSION_BOUND,
	NET_SESSION_CONNECTING,
	NET_SESSION_JOINING,
	NET_SESSION_READY,
	NUM_NET_SESSION_STATES
};

enum eNetworkSessionError
{
	NET_SESSION_OK = 0,
	NET_SESSION_ERROR_USER,
	NET_SESSION_ERROR_INTERNAL,
	
	NET_SESSION_ERROR_JOIN_DENIED,
	NET_SESSION_ERROR_JOIN_DENIED_NOT_HOST,
	NET_SESSION_ERROR_JOIN_DENIED_CLOSED,
	NET_SESSION_ERROR_JOIN_DENIED_FULL,
	NUM_NET_SESSION_ERRORS
};


//------------
// Structures
//
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


//--------
// Classes
//
class NetworkSession
{
public:
	 NetworkSession( Renderer *currentRenderer = nullptr );
	~NetworkSession();

private:
	// My Socket
	UDPSocket			*m_mySocket			= nullptr;

	// Me & My Host
	NetworkConnection	*m_myConnection		= nullptr;
	NetworkConnection	*m_hostConnection	= nullptr;

public:
	// My Connections
	NetworkConnectionsVector	 m_allConnections;								// Vector of all the connections
	NetworkConnectionsArray		 m_boundConnections;
	NetworkMessageDefinition*	 m_registeredMessages[ 256 ]	= { nullptr };	// Array NetworkMessageDefinition: index is uint8 => [0, 0xff]

	// My State
	eNetworkSessionState		 m_state;
	eNetworkSessionError		 m_errorCode;
	std::string					 m_errorString;

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
	void Update();
	void Render() const;

	void ProcessIncoming();
	void ProcessOutgoing();

private:
	// Network Operations
	bool					BindPort( uint16_t port, uint16_t range );
	void					RegisterCoreMessages();

public:
	void					Host( char const *myID, uint16_t port, uint16_t portRange = DEFAULT_PORT_RANGE );
	void					Join( char const *myID, NetworkConnectionInfo const &hostInfo );
	void					Disconnect();

	void					SetError( eNetworkSessionError error, char const *str );
	void					ClearError();
	eNetworkSessionError	GetLastError( std::string *outStr );

	void					SendPacket			( NetworkPacket &packetToSend );		// Replaces connectionIndex by sender's index
	void					SendDirectMessageTo	( NetworkMessage &messageToSend, NetworkAddress const &address );

private:
	NetworkConnection*		CreateConnection	( NetworkConnectionInfo const &info );
	void					DestroyConnection	( NetworkConnection *connection );
	void					BindConnection		( uint8_t idx, NetworkConnection *connection );

	uint8_t					GetMyConnectionIndex() const;				// Returns 0xff if not found
	NetworkConnection*		GetMyConnection();							// Will crashes if not found

public:
	// Connections & Messages
	NetworkConnection*	GetConnection( int idx );

	bool				RegisterNetworkMessage( char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netMessageOptionsFlag );					// Returns true on success
	void				RegisterNetworkMessage( uint8_t index, char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netmessageOptionsFlag );	// Rewrite if a definition already exists at that index

	NetworkMessageDefinition const* GetRegisteredMessageDefination( std::string const &definitionName ) const;															// Returns -1 if not found
	NetworkMessageDefinition const* GetRegisteredMessageDefination( int defIndex ) const;

private:
	// Net Simulation
	float			m_simulatedLossFraction	 = 0.f;
	uint			m_simulatedMinLatency_ms = 0U;
	uint			m_simulatedMaxLatency_ms = 0U;
	uint8_t			m_simulatedSendFrequency = 20;
	float			m_heartbeatFrequency	 = 0.2f;

private:
	// Priority Queue
	StampedNetworkPacketPriorityQueue m_receivedPackets;

public:
	inline float	GetHeartbeatFrequency()		const { return m_heartbeatFrequency; }
	inline float	GetSimulatedLossFraction()	const { return m_simulatedLossFraction; }
	inline uint8_t	GetSimulatedSendFrequency()	const { return m_simulatedSendFrequency; }

	bool			SetHeartbeatFrequency( float frequencyHz );
	void			SetSimulationLoss( float lossFraction );
	void			SetSimulationLatency( uint minAddedLatency_ms, uint maxAddedLatency_ms = 0U );
	void			SetSimulationSendFrequency( uint8_t frequencyHz );

private:
	void			ReceivePacket();
	void			ProcessReceivedPackets();
	void			QueuePacketForSimulation( NetworkPacket *newPacket, NetworkAddress &sender );	// Queues them with a random latency
	void			ProccessAndDeletePacket ( NetworkPacket *&packet, NetworkAddress &sender );	// Process the packet and deletes it
};
