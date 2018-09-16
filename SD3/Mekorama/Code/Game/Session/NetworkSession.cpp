#pragma once
#include "NetworkSession.hpp"

bool NetworkSender::Send( NetworkMessage &msgToSend ) const
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

void NetworkSession::ProcessIncoming()
{

}

void NetworkSession::ProcessOutgoing()
{

}
