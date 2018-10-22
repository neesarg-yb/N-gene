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

public:
	uint64_t			 m_lastSendTimeHPC				= Clock::GetCurrentHPC();	// Analytics
	uint64_t			 m_lastReceivedTimeHPC			= Clock::GetCurrentHPC();

	float				 m_loss	= 0.f;												// [0, 1] Loss rate we perceive to this connection
	float				 m_rtt	= 0.f;												// Latency perceived on this connection

private:
	// Outgoing Messages
	uint8_t				 m_sendFrequency = 0xff;		// Defaults to max frequency: 255hz 
	NetworkMessages		 m_outgoingUnreliableMessages;	// Unreliable messages, for now

	// Tracking the packets
	PacketTracker		 m_packetTrackers[ MAX_TRACKED_PACKETS ];

private:
	// Timers
	Stopwatch			 m_sendRateTimer;				// To simulate send frequency
	Stopwatch			 m_heartbeatTimer;				// To send heartbeat regularly

public:
	void	OnReceivePacket( NetworkPacketHeader receivedPacketHeader );
	void	ConfirmPacketReceived( uint16_t ack );

	void	Send( NetworkMessage &msg );
	void	FlushMessages();

	uint8_t	GetCurrentSendFrequency() const;			// Minimum of ( My sendFrequency, Parent's sendFrequency )
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
	void	UpdateHeartbeatTimer();						// Sets the heartbeat timer according to parentSession

private:
	uint16_t GetNextAckToSend();
	void	 IncrementSentAck();

	PacketTracker*	AddTrackedPacket( uint16_t ack );
	bool			IsActivePacketTracker( uint16_t ack );
};
