#pragma once
#include "NetworkMessageChannel.hpp"

NetworkMessageChannel::NetworkMessageChannel()
{

}

NetworkMessageChannel::~NetworkMessageChannel()
{
	while ( m_pendingMessagesToProcess.size() > 0 )
	{
		delete m_pendingMessagesToProcess.top().message;
		delete m_pendingMessagesToProcess.top().sender;

		m_pendingMessagesToProcess.pop();
	}
}

