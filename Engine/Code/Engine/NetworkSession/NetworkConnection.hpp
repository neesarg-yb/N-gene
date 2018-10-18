#pragma once#
#include <vector>
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

#define INVALID_PACKET_ACK (0xffff)

class NetworkSession;

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

private:
	// Packet Tracking
	uint16_t			 m_nextSentAck		= 0U;					// Sending		- Updated during a flush
	uint16_t			 m_lastReceivedAck	= INVALID_PACKET_ACK;	// Receiving	- Updated during process packet
	uint16_t			 m_lastSendTime_ms;							// Analytics
	uint16_t			 m_lastReceivedTime_ms;

	float				 m_loss	= 0.f;								// Loss rate we perceive to this connection
	float				 m_rtt	= 0.f;								// Latency perceived on this connection

private:
	// Outgoing Messages
	uint8_t				 m_sendFrequency = 0xff;		// Defaults to max frequency: 255hz 
	NetworkMessages		 m_outgoingUnreliableMessages;	// Unreliable messages, for now

private:
	// Timers
	Stopwatch			 m_sendRateTimer;			// To simulate send frequency
	Stopwatch			 m_heartbeatTimer;			// To send heartbeat regularly

public:
	void	Send( NetworkMessage &msg );
	void	FlushMessages();

	uint8_t	GetCurrentSendFrequency() const;			// Minimum of ( My sendFrequency, Parent's sendFrequency )
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
	void	UpdateHeartbeatTimer();						// Sets the heartbeat timer according to parentSession
};
