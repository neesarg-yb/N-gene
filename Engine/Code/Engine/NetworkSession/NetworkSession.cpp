#pragma once
#include "NetworkSession.hpp"

NetworkSession::NetworkSession()
{

}

NetworkSession::~NetworkSession()
{

}

bool NetworkSession::BindPort( uint16_t port, uint range )
{
	UNUSED( port );
	UNUSED( range );

	return false;
}

void NetworkSession::ProcessIncoming()
{

}

void NetworkSession::ProcessOutgoing()
{

}

NetworkConnection* NetworkSession::AddConnection( int idx, NetworkAddress &addr )
{
	// Delete the existing one
	if( idx < m_connections.size() )
	{
		delete m_connections[idx];
		m_connections[idx] = nullptr;
	}

	m_connections[idx] = new NetworkConnection( idx, addr, *this );
	return m_connections[idx];
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	if( idx < m_connections.size() )
		return nullptr;
	else
		return m_connections[idx];
}

void NetworkSession::RegisterNetworkMessage( char const *messageName, networkMessage_cb cb )
{
	// Register new message
	m_registeredMessages[ messageName ] = NetworkMessageDefinition( messageName, cb );

	// Adjust each messages' ID
	int messageID = 0;
	for( NetworkMessageDefinitionsMap::iterator	it  = m_registeredMessages.begin(); 
												it != m_registeredMessages.end(); 
												it++ )
	{
		it->second.id = messageID;
		messageID++;
	}
}
