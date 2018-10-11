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
	Stopwatch			 m_timer;					// To simulate send frequency

public:
	void Send( NetworkMessage &msg );
	void FlushMessages();

	uint8_t	GetSendFrequency() const;
	void	SetSendFrequencyTo( uint8_t frequencyHz );	// Sets it to the min( passedFrequency, parentsFrequency )
};
