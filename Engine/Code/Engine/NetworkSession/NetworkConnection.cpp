#pragma once
#include "NetworkConnection.hpp"
#include "Engine/NetworkSession/NetworkSession.hpp"

NetworkConnection::NetworkConnection( int idx, NetworkAddress &addr, NetworkSession &parentSession )
	: m_indexInSession( idx )
	, m_parentSession( parentSession )
	, m_address( addr )
{

}

NetworkConnection::~NetworkConnection()
{

}

void NetworkConnection::ProcessOutgoing()
{

}
