#pragma once#
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

class  NetworkSession;
struct NetworkPacketHeader;

typedef std::vector< NetworkMessage* > NetworkMessages;

class NetworkConnection
{
public:
	 NetworkConnection( int idx, NetworkAddress &addr, NetworkSession &parentSession );
	~NetworkConnection();

public:
	// Connection Info.
	NetworkAddress		 m_address;
	NetworkSession		&m_parentSession;
	int					 m_indexInSession;

public:
	// Packet Tracking
	uint16_t			 m_nextSentAck					= INVALID_PACKET_ACK;		// Sending		- Updated during a flush
	uint16_t			 m_highestReceivedAck			= INVALID_PACKET_ACK;		// Receiving	- Updated during process packet
	uint16_t			 m_receivedAcksBitfield			= 0U;
	uint16_t			 m_nextSentReliableID			= INVALID_RELIABLE_ID;

public:
	uint64_t			 m_lastSendTimeHPC				= Clock::GetCurrentHPC();	// Analytics
	uint64_t			 m_lastReceivedTimeHPC			= Clock::GetCurrentHPC();

	float				 m_loss	= 0.f;												// [0, 1] Loss rate we perceive to this connection
	float				 m_rtt	= 0.f;												// Latency perceived on this connection

private:
	// Outgoing Messages
	bool				 m_immediatlyRespondForAck	= false;
	uint8_t				 m_sendFrequency			= 0xff;							// Defaults to max frequency: 255hz 
	NetworkMessages		 m_outgoingUnreliables;										// Unreliable messages, for now

	// Tracking the packets
	PacketTracker		  m_packetTrackers[ MAX_TRACKED_PACKETS ];
	std::vector<uint16_t> m_receivedReliableIDs;

	// Sent Messages
	NetworkMessages		 m_outgoingReliables;
	NetworkMessages		 m_unconfirmedSentReliables;

private:
	// Timers
	Stopwatch			 m_sendRateTimer;				// To simulate send frequency
	Stopwatch			 m_heartbeatTimer;				// To send heartbeat regularly
	Stopwatch			 m_confirmReliablesTimer;

public:
	// Receiving End
	void	OnReceivePacket( NetworkPacketHeader receivedPacketHeader );		// It is there for tracking the messages & packets, it doesn't process em!
	void	ProcessReceivedMessage( NetworkMessage &receivedMessage, NetworkSender sender );

	// Sending End
	bool	HasNewMessagesToSend() const;				// New reliables or unreliable, not the unconfirmed ones
	void	Send( NetworkMessage &msg );				// Queues the messages to send
	void	FlushMessages();							// Sends the queued messages

	// Current State
	uint	GetUnconfirmedSendReliablesCount() const;
	uint8_t	GetCurrentSendFrequency() const;			// Minimum of ( My sendFrequency, Parent's sendFrequency )
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
	void	UpdateHeartbeatTimer();						// Sets the heartbeat timer according to parentSession

private:
	// Tracking Messages-or-Packet
	void			ConfirmPacketReceived( uint16_t ack );
	bool			ReliableMessageAlreadyReceived( uint16_t reliableID );	// If not, returns false and adds it to the received list; returns true if already received

	// Acks
	uint16_t		GetNextAckToSend();
	void			IncrementSentAck();
	float			CalculateLoss() const;									// Goes through the packetTrackers & calculates loss according to how many packets are still being tracked

	PacketTracker*	AddTrackedPacket( uint16_t ack );
	bool			IsActivePacketTracker( uint16_t ack );
	void			EmptyTheOutgoingUnreliables();

	// Reliable ID
	uint16_t		GetNextReliableIDToSend();
	void			IncrementSentReliableID();
	bool			ShouldResendUnconfirmedReliables();						// "Checks-and-Resets" the timer. Returns true if it is time to resend unconfirmed reliables (if there are any)   [ "USE IT JUST ONCE!!" ]
	void			ConfirmReliableMessages( PacketTracker &tracker );		// Removes the confirmed messages from m_unconfirmedSentReliables
};
