#pragma once
#include "NetworkConnection.hpp"
#include "Engine/NetworkSession/NetworkSession.hpp"

NetworkConnection::NetworkConnection( NetworkAddress &addr, NetworkSession &parentSession )
	: connection( addr )
	, session( parentSession )
{

}

NetworkConnection::~NetworkConnection()
{

}

MessageQueueElement::MessageQueueElement( NetworkAddress &connectionAddr, NetworkMessage &msg )
	: connection( connectionAddr )
	, message( msg )
	, operationComplete( false )
{

}

void NetworkConnection::Send( NetworkMessage &msgToSend )
{
	session.QueueMessageToSend( msgToSend, connection );
}