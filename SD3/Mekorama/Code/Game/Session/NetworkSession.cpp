#pragma once
#include "NetworkSession.hpp"

bool NetworkConnection::Send( NetworkMessage &msgToSend ) const
{
	UNUSED( msgToSend );
	return false;
}

NetworkSession::NetworkSession()
{

}

NetworkSession::~NetworkSession()
{

}

void NetworkSession::AddBinding( uint16_t port )
{
	UNUSED( port );
}

void NetworkSession::RegisterMessage( char const *messageName, sessionMessage_cb cb )
{
	UNUSED( messageName );
	UNUSED( cb );
}

NetworkConnection* NetworkSession::AddConnection( int idx, NetworkAddress &addr )
{
	UNUSED( idx );
	UNUSED( addr );

	return nullptr;
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	UNUSED( idx );

	return nullptr;
}

void NetworkSession::ProcessIncoming()
{

}

void NetworkSession::ProcessOutgoing()
{

}
