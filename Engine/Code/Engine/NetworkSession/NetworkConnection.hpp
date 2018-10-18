#pragma once#
#include <vector>
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

class NetworkSession;

typedef std::vector< NetworkMessage* > NetworkMessages;

class NetworkConnection
{
public:
	 NetworkConnection( int idx, NetworkAddress &addr, NetworkSession &parentSession );
	~NetworkConnection();

public:
	NetworkAddress		 m_address;
	NetworkSession		&m_parentSession;
	int					 m_indexInSession;

private:
	uint8_t				 m_sendFrequency = 0xff;	// Defaults to max frequency: 255hz 
	NetworkMessages		 m_outgoingMessages;		// Unreliable messages, for now

private:
	Stopwatch			 m_sendRateTimer;			// To simulate send frequency
	Stopwatch			 m_heartbeatTimer;			// To send heartbeat regularly

public:
	void Send( NetworkMessage &msg );
	void FlushMessages();

	uint8_t	GetCurrentSendFrequency() const;			// Minimum of ( My sendFrequency, Parent's sendFrequency )
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
	void	UpdateHeartbeatTimer();						// Sets the heartbeat timer according to parentSession
};
