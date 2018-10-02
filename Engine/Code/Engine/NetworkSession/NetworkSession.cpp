#pragma once
#include "NetworkSession.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

NetworkSession::NetworkSession()
{

}

NetworkSession::~NetworkSession()
{
	// Delete all connections
	while ( m_connections.size() > 0U )
	{
		// Fast delete
		std::swap( m_connections.front(), m_connections.back() );
		delete m_connections.back();
		m_connections.back() = nullptr;

		m_connections.pop_back();
	}

	// Delete my UDP Socket
	delete m_mySocket;
	m_mySocket = nullptr;
}

bool NetworkSession::BindPort( uint16_t port, uint16_t range )
{
	NetworkAddress localAddress = NetworkAddress::GetLocal();
	localAddress.port = port;

	// If socket already exists, delete the old one
	if( m_mySocket != nullptr )
	{
		delete m_mySocket;
		m_mySocket = nullptr;
	}

	// Create a new socket and try to bind
	m_mySocket = new UDPSocket();
	bool success = m_mySocket->Bind( localAddress, range );

	// return true on success
	if( success )
		return true;

	// on failure, return after deleting the socket
	delete m_mySocket;
	m_mySocket = nullptr;
	return false;
}

void NetworkSession::ProcessIncoming()
{
	NetworkAddress sender;

	void	*buffer			= malloc( PACKET_MTU );
	size_t	 receivedBytes	= m_mySocket->ReceiveFrom( &sender, buffer, PACKET_MTU );

	NetworkPacket receivedPacket;
	receivedPacket.WriteBytes( receivedBytes, buffer, false );

	if( receivedBytes > 0 )
	{
		if( receivedPacket.IsValid() )
		{
			bool headerReadingSuccess = receivedPacket.ReadHeader( receivedPacket.m_header );
			GUARANTEE_RECOVERABLE( headerReadingSuccess, "Couldn't read the Packet Header successfully!" );

			NetworkMessage receivedMessage;
			for( int i = 0; i < receivedPacket.m_header.unreliableMessageCount; i++ )
			{
				bool messageReadSuccess = receivedPacket.ReadMessage( receivedMessage );
				GUARANTEE_RECOVERABLE( messageReadSuccess, "Couldn't read the Network Message successfully!" );

				// To get Message Definition from index
				NetworkMessageDefinitionsMap::iterator it = m_registeredMessages.begin();
				std::advance( it, receivedMessage.m_header.networkMessageDefinitionIndex );

				// If that's a valid definition index
				if( it != m_registeredMessages.end() )
				{
					// Set the pointer to that definition
					receivedMessage.m_definition = &it->second;
					
					// Create a NetworkSender
					NetworkSender thisSender = NetworkSender( *this, sender, nullptr );
					uint8_t receivedConnIdx = receivedPacket.m_header.connectionIndex;
					if( receivedConnIdx != 0xff )
						thisSender.connection = m_connections[ receivedPacket.m_header.connectionIndex ];		// If sender has a valid connection, fill it in

					// Do a callback!
					receivedMessage.m_definition->callback( receivedMessage, thisSender );
				}
				else
					ConsolePrintf( "Received invalid messageDefinition Index: %d", receivedMessage.m_header.networkMessageDefinitionIndex );
			}
		}
		else
			ConsolePrintf( RGBA_KHAKI_COLOR, "Bad Packet Received from %s", sender.AddressToString().c_str() );
	}

	// Free the temp buffer
	free( buffer );
}

void NetworkSession::ProcessOutgoing()
{
	for each (NetworkConnection* connection in m_connections)
		connection->FlushMessages();
}

void NetworkSession::SendPacket( NetworkPacket &packetToSend )
{
	uint8_t idx = packetToSend.m_header.connectionIndex;
	packetToSend.m_header.connectionIndex = GetMyConnectionIndex();
	packetToSend.WriteHeader( packetToSend.m_header );

	m_mySocket->SendTo( m_connections[idx]->m_address, packetToSend.GetBuffer(), packetToSend.GetWrittenByteCount() );
}

void NetworkSession::SendDirectMessageTo( NetworkMessage &messageToSend, NetworkAddress const &address )
{
	NetworkPacket packetToSend;
	messageToSend.m_header.networkMessageDefinitionIndex = (uint8_t) m_registeredMessages[ messageToSend.m_name ].id;
	packetToSend.WriteMessage( messageToSend );

	m_mySocket->SendTo( address, packetToSend.GetBuffer(), packetToSend.GetWrittenByteCount() );
}

uint8_t NetworkSession::GetMyConnectionIndex() const
{
	// defaults to INVALID index
	uint8_t idx = 0xff;

	for( uint i = 0; i < m_connections.size(); i++ )
	{
		// If connection found..
		if( m_connections[i]->m_address == m_mySocket->m_address )
		{
			// If it is under max indices allowed
			if( i < 0xff )
				idx = (uint8_t)i;

			break;
		}
	}

	return idx;
}

NetworkConnection* NetworkSession::AddConnection( int idx, NetworkAddress &addr )
{
	TODO( "Make it such that you can skip indices while adding a new conncetion!" );

	NetworkConnection* thisConnection = new NetworkConnection( idx, addr, *this );
	if( idx < m_connections.size() )
	{
		// Delete the existing one
		delete m_connections[idx];
		m_connections[idx] = nullptr;

		// Replace it with the new one
		m_connections[idx] = thisConnection;
	}
	else
	{
		// Add a new connection at the end
		thisConnection->m_indexInSession = (int)m_connections.size();		// Set the new index
		m_connections.push_back( thisConnection );
	}

	return m_connections[idx];
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	if( idx >= m_connections.size() )
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
