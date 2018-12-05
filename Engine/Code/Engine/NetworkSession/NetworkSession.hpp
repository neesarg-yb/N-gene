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
// Enums
//
enum eNetworkSessionState : uint8_t
{
	NET_SESSION_DISCONNECTED = 0,			// Session can be modified
	NET_SESSION_BOUND,						// Bound to a socket, but no connections exists. Can send and receive connectionless messages. 
	NET_SESSION_CONNECTING,					// Attempting to connect with host, no response received yet; i.e. waiting.
	NET_SESSION_JOINING,					// Has established a connection, waiting for final setup information or join completion
	NET_SESSION_READY,						// We are fully ready
	NUM_NET_SESSION_STATES
};

std::string ToString( eNetworkSessionState inEnum );

enum eNetworkSessionError : uint8_t
{
	NET_SESSION_OK = 0,						// No errors
	NET_SESSION_ERROR_USER,					// User disconnected
	NET_SESSION_ERROR_INTERNAL,				// Socket error
	NET_SESSION_ERROR_TIMEOUT,				// Connection didn't respond for too long
	
	NET_SESSION_ERROR_JOIN_DENIED,			// Generic deny error (release)
	NET_SESSION_ERROR_JOIN_DENIED_NOT_HOST,	// Debug - tried to join someone who isn't joining
	NET_SESSION_ERROR_JOIN_DENIED_CLOSED,	// Debug - not in a listen state
	NET_SESSION_ERROR_JOIN_DENIED_FULL,		// Debug - Session was full :(
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
	UDPSocket			*m_mySocket				= nullptr;

	// Me & My Host
	NetworkConnection	*m_myConnection			= nullptr;
	NetworkConnection	*m_hostConnection		= nullptr;

	// Timers
	double const		 m_joinTimerSeconds		= 0.1;
	Stopwatch			 m_joinRequestTimer;

	double const		 m_joinTimeoutSeconds	= 10.0;
	Stopwatch			 m_joinTimeoutTimer;

public:
	// My Connections
	NetworkConnectionsVector	 m_allConnections;								// Vector of all the connections
	NetworkConnectionsArray		 m_boundConnections;
	NetworkMessageDefinition*	 m_registeredMessages[ 256 ]	= { nullptr };	// Array NetworkMessageDefinition: index is uint8 => [0, 0xff]

	// My State
	eNetworkSessionState		 m_state		= NET_SESSION_DISCONNECTED;
	eNetworkSessionError		 m_errorCode	= NET_SESSION_OK;
	std::string					 m_errorString	= "";

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
	// Core Operations
	void Update();
	void Render() const;

private:
	void UpdateSessionDisconnected();
	void UpdateSessionBound();
	void UpdateSessionConnecting();
	void UpdateSessionJoining();
	void UpdateSessionReady();

	void UpdateStateTo( eNetworkSessionState newState );								// Updates state and resets the timers for new state

public:
	void ProcessIncoming();
	void ProcessOutgoing();

private:
	// Receiving & Processing
	void ReceivePacket();
	void ProcessReceivedPackets();
	void QueuePacketForSimulation( NetworkPacket *newPacket, NetworkAddress &sender );	// Queues them with a random latency
	void ProccessAndDeletePacket ( NetworkPacket *&packet, NetworkAddress &sender );	// Process the packet and deletes it

public:
	// Sending
	void SendPacket			( NetworkPacket &packetToSend );							// Replaces connectionIndex by sender's index
	void SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address );
	void BroadcastMessage	( NetworkMessage &messageToBroadcast, NetworkConnection const *excludeConnection = nullptr );

private:
	// Session Setup
	bool BindPort( uint16_t port, uint16_t range );

public:
	void Host( char const *myID, uint16_t port, uint16_t portRange = DEFAULT_PORT_RANGE );
	void Join( char const *myID, NetworkAddress const &hostAddress );
	void Disconnect();

	bool ProcessJoinRequest				( char *networkID, NetworkAddress const &reqFromAddress );
	bool ProcessJoinDeny				( eNetworkSessionError errorCode, NetworkAddress const &senderAddress );
	bool ProcessJoinAccept				( uint8_t connectionIdx, NetworkAddress const &senderAddress );
	bool ProcessJoinFinished			( NetworkAddress const &senderAddress );
	bool ProcessUpdateConnectionState	( eNetworkConnectionState state, NetworkAddress const &senderAddress );

	// Session Errors
	void					SetError( eNetworkSessionError error, char const *str );
	void					ClearError();
	eNetworkSessionError	GetLastError( std::string *outStr );

	// Current State
	inline bool				IsRunning()		const { return m_state != NET_SESSION_DISCONNECTED; }
	inline bool				IsHosting()		const { return m_myConnection == m_hostConnection; }
	inline bool				IsListening()	const { return m_state == NET_SESSION_READY; }
	bool					IsLobbyFull()	const;

private:
	// Network Connections
	NetworkConnection*		CreateConnection	( NetworkConnectionInfo const &info );
	void					BindConnection		( uint8_t idx, NetworkConnection *connection );
	void					DeleteConnection	( NetworkConnection* connection );		// Deletes the connection and removes the connection from: m_myConnection, m_hostConnection, m_allConnections & m_boundConnections
	void					DeleteAllConnections();										// Including myConnection & hostConnection
	void					RemoveDisconnectedConnections();
	
	void					SetBoundConnectionsToNull();								// Deletes and sets all bound connections to nullptr
	bool					ConnectionAlreadyExists	( NetworkAddress const &address );
	int						GetIndexForNewConnection() const;							// Returns -1, if no vacant slots found

public:
	NetworkConnection*		GetConnection( int idx );
	NetworkConnection*		GetConnection( NetworkAddress const &address );
	uint8_t					GetMyConnectionIndex() const;
	bool					IsRegistered( NetworkConnection const *connection ) const;	// connection is in m_allConnections
	
	inline NetworkConnection* const GetMyConnection()	const { return m_myConnection; }
	inline NetworkConnection* const GetHostConnection()	const { return m_hostConnection; }

	// Message Definitions
	void					RegisterCoreMessages();
	bool					RegisterNetworkMessage( char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netMessageOptionsFlag );					// Returns true on success
	void					RegisterNetworkMessage( uint8_t index, char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netmessageOptionsFlag );	// Rewrite if a definition already exists at that index

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
	StampedNetworkPacketPriorityQueue m_receivedPackets;				// Priority Queue

public:
	inline float	GetHeartbeatFrequency()		const { return m_heartbeatFrequency; }
	inline float	GetSimulatedLossFraction()	const { return m_simulatedLossFraction; }
	inline uint8_t	GetSimulatedSendFrequency()	const { return m_simulatedSendFrequency; }

	bool			SetHeartbeatFrequency( float frequencyHz );
	void			SetSimulationLoss( float lossFraction );
	void			SetSimulationLatency( uint minAddedLatency_ms, uint maxAddedLatency_ms = 0U );
	void			SetSimulationSendFrequency( uint8_t frequencyHz );
};
