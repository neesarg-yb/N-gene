#pragma once#
#include <string>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkMessageChannel.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"


//---------------------------------
// Forward Declarations
// 
class  NetworkSession;
struct NetworkPacketHeader;

typedef std::vector< NetworkMessage* > NetworkMessages;


//---------------------------------
// Enums
// 
enum eNetworkConnectionState : uint8_t
{
	NET_CONNECTION_DISCONNECTED = 0,
	NET_CONNECTION_CONNECTING,
	NET_CONNECTION_CONNECTED,
	NET_CONNECTION_READY,
	NUM_NET_CONNECTIONS
};

std::string ToString( eNetworkConnectionState inEnum );


//---------------------------------
// Structures
// 
struct NetworkConnectionInfo
{
public:
	NetworkAddress	address;												// My address..
	uint8_t			indexInSession;											// Index in the session
	char			networkID[ MAX_NETWORK_ID_LENGTH + 1 ] = "INVALID";		// Like a Steam or PSN ID

public:
	NetworkConnectionInfo( int index, std::string const &desiredNetworkID, NetworkAddress const &addr );

public:
	bool operator == ( NetworkConnectionInfo const &b ) const;
};


//---------------------------------
// Classes
// 
class NetworkConnection
{
public:
	 NetworkConnection( NetworkConnectionInfo const &info, NetworkSession &parentSession );
	 NetworkConnection( int idx, NetworkAddress const &addr, std::string networkID, NetworkSession &parentSession );
	~NetworkConnection();

public:
	// Connection Info.
	NetworkSession			&m_parentSession;
	NetworkConnectionInfo	 m_info;

private:
	eNetworkConnectionState	 m_state					= NET_CONNECTION_DISCONNECTED;

public:
	// Packet Tracking
	uint16_t			 m_nextSentAck					= INVALID_PACKET_ACK;		// Sending		- Updated during a flush
	uint16_t			 m_highestReceivedAck			= INVALID_PACKET_ACK;		// Receiving	- Updated during process packet
	uint16_t			 m_receivedAcksBitfield			= 0U;
	uint16_t			 m_nextReliableIDToSend			= 0x0000;
	uint16_t			 m_highestConfirmedReliableID	= 0xffff;

public:
	uint64_t			 m_lastSendTimeHPC				= Clock::GetCurrentHPC();	// Analytics
	uint64_t			 m_lastReceivedTimeHPC			= Clock::GetCurrentHPC();

	float				 m_loss	= 0.f;												// [0, 1] Loss rate we perceive to this connection
	float				 m_rtt	= 0.f;												// IN SECONDS; Latency perceived on this connection

private:
	// Outgoing-and-Sent Messages
	bool				 m_immediatlyRespondForAck	= false;
	uint8_t				 m_sendFrequency			= 0xff;							// Defaults to max frequency: 255hz 
	NetworkMessages		 m_outgoingUnreliables;										// Unreliable messages, for now
	NetworkMessages		 m_outgoingReliables;
	NetworkMessages		 m_unconfirmedSentReliables;

	// Network Channels
	NetworkMessageChannel m_messageChannels[ MAX_NETWORK_MESSAGE_CHANNELS ];

	// Tracking the packets
	PacketTracker		  m_packetTrackers[ MAX_TRACKED_PACKETS ];
	std::vector<uint16_t> m_receivedReliableIDs;

private:
	// Timers
	Stopwatch			 m_sendRateTimer;				// To simulate send frequency
	Stopwatch			 m_heartbeatTimer;				// To send heartbeat regularly
	Stopwatch			 m_confirmReliablesTimer;

public:
	bool operator == ( NetworkConnection const &b ) const;
	bool operator != ( NetworkConnection const &b ) const;

public:
	inline NetworkAddress	GetAddress()		const { return m_info.address; }
	inline uint8_t			GetIndexInSession()	const { return m_info.indexInSession; }
	inline std::string		GetNetworkID()		const { return m_info.networkID; }

	// Connection State
	eNetworkConnectionState	GetState() const;
	void					UpdateStateTo( eNetworkConnectionState newState, bool broadcast );

	// Receiving End
	void	OnReceivePacket( NetworkPacketHeader receivedPacketHeader );		// It is there for tracking the messages & packets, it doesn't process em!
	void	ProcessReceivedMessage( NetworkMessage &receivedMessage, NetworkSender sender );

	// Sending End
	bool	HasNewMessagesToSend() const;				// New reliables or unreliable, not the unconfirmed ones
	void	Send( NetworkMessage &msg );				// Queues the messages to send
	void	FlushMessages( bool ignoreSendRate = false );							// Sends the queued messages

	// Current State - Messages
	uint16_t GetLowestReliableIDToConfirm() const;
	uint16_t GetHighestConfirmedReliableID() const;
	bool	 HasReceivedReliableID( uint16_t reliableID ) const;

	uint	GetUnconfirmedSendReliablesCount() const;
	uint8_t	GetCurrentSendFrequency() const;			// Minimum of ( My sendFrequency, Parent's sendFrequency )
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
	void	UpdateHeartbeatTimer();						// Sets the heartbeat timer according to parentSession

	// Connection Identification
	bool	IsMe()		const;							// If this is the local connection
	bool	IsHost()	const;							// If this connection is the host
	bool	IsClient()	const;							// If this connection is a client

	// Current State - Connection
	inline bool		IsConnected()	 const { return (m_state == NET_CONNECTION_CONNECTED) || (m_state == NET_CONNECTION_READY); }
	inline bool		IsDisconnected() const { return  m_state == NET_CONNECTION_DISCONNECTED; }
	inline bool		IsReady()		 const { return  m_state == NET_CONNECTION_READY; }

private:
	// Tracking Messages-or-Packet
	void			ConfirmPacketReceived( uint16_t ack );
	bool			CanSendReliableID( uint16_t reliableID );

	// Acks
	uint16_t		GetNextAckToSend();
	void			IncrementSentAck();
	float			CalculateLoss() const;									// Goes through the packetTrackers & calculates loss according to how many packets are still being tracked

	PacketTracker*	AddTrackedPacket( uint16_t ack );
	bool			IsActivePacketTracker( uint16_t ack );
	void			EmptyTheOutgoingUnreliables();

	// Reliable ID
	uint16_t		GetNextReliableIDToSend();
	void			IncrementReliableIDToSend();
	bool			ShouldResendUnconfirmedReliables();						// "Checks-and-Resets" the timer. Returns true if it is time to resend unconfirmed reliables (if there are any)   [ "USE IT JUST ONCE!!" ]
	void			UpdateHigestConfirmedReliableID( PacketTracker &tracker );		// Removes the confirmed messages from m_unconfirmedSentReliables
	void			MarkReliableReceived( uint16_t reliableID );
};
