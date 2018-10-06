#pragma once
#include "NetworkSession.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

NetworkSession::NetworkSession( Renderer *currentRenderer /* = nullptr */ )
	: m_theRenderer( currentRenderer )
{
	// For UI
	m_uiCamera = new Camera();

	// Setting up the Camera
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC

	if( currentRenderer != nullptr )
		m_fonts = currentRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
}

NetworkSession::~NetworkSession()
{
	// Delete all connections
	for( size_t i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		// If nullptr, skip
		if( m_connections[i] == nullptr )
			continue;

		// Delete
		delete m_connections[i];
		m_connections[i] = nullptr;
	}

	// Delete my UDP Socket
	delete m_mySocket;
	m_mySocket = nullptr;
}

void NetworkSession::Render() const
{
	m_theRenderer->BindCamera( m_uiCamera );

	// To form an overlay: do not clear screen, make depth of every pixel 1.f, do not write new depth..
	m_theRenderer->UseShader( nullptr );
	m_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	// Draw overlay
	AABB2 backgroundBox = m_screenBounds.GetBoundsFromPercentage( Vector2( 0.f, 0.8f ), Vector2( 0.2f, 1.f ) );
	m_theRenderer->DrawAABB( backgroundBox, m_uiBackgroundColor );

	// Title Box
	AABB2		titleBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.f, 0.9f ), Vector2( 1.f, 1.f ) );
	std::string	titleStr = "NETWORK SESSION";
	m_theRenderer->DrawTextInBox2D( titleStr.c_str(), Vector2( 0.f, 0.5f ), titleBox, 0.03f, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// Simulated Rate, Lag & Loss
	AABB2		srllBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.1f, 0.6f ), Vector2( 1.f, 0.9f ) );
	std::string srllStr = Stringf( "%-8s:%s\n%-8s:%s\n%-8s:%s", "rate", "XX hz", "sim_lag", "X ms-X ms", "sim_loss", "X.XX %" );
	m_theRenderer->DrawTextInBox2D( srllStr.c_str(), Vector2( 0.f, 1.f ), srllBox, 0.025f, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// My Socket Address
	AABB2		myAddressBaseBox	= backgroundBox.GetBoundsFromPercentage   ( Vector2( 0.0f, 0.5f ), Vector2( 1.f, 0.7f ) );
	AABB2		myAddressTitleBox	= myAddressBaseBox.GetBoundsFromPercentage( Vector2( 0.0f, 0.5f ), Vector2( 1.f, 1.0f ) );
	AABB2		myAddressBox		= myAddressBaseBox.GetBoundsFromPercentage( Vector2( 0.1f, 0.0f ), Vector2( 1.f, 0.5f ) );
	std::string myAddressTitle		= "My Socket Address:";
	std::string socketAddrStr		= m_mySocket->m_address.AddressToString();
	m_theRenderer->DrawTextInBox2D( myAddressTitle.c_str(), Vector2( 0.f, 0.5f ), myAddressTitleBox, 0.025f, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
	m_theRenderer->DrawTextInBox2D( socketAddrStr.c_str(),  Vector2( 0.f, 0.5f ), myAddressBox,      0.025f, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// Connections Heading
	AABB2		connectionsHeadingBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.0f, 0.4f ), Vector2( 1.f, 0.5f ) );
	std::string connectionsHeadingStr = "Connections:";
	m_theRenderer->DrawTextInBox2D( connectionsHeadingStr.c_str(), Vector2( 0.f, 0.5f ), connectionsHeadingBox, 0.025f, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
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
	for( int i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		// If nullptr, skip
		if( m_connections[i] == nullptr )
			continue;

		m_connections[i]->FlushMessages();
	}
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

	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_connections[i] == nullptr )
			continue;

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
	// If idx is not in range
	if( idx < 0 || idx >= MAX_SESSION_CONNECTIONS )
		return nullptr;

	// If there's a connection there already, delete it
	if( m_connections[idx] != nullptr )
	{
		delete m_connections[idx];
		m_connections[idx] = nullptr;
	}

	// Set new connection to that index
	NetworkConnection* thisConnection = new NetworkConnection( idx, addr, *this );
	m_connections[idx] = thisConnection;

	return m_connections[idx];
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	if( idx < 0 || idx >= MAX_SESSION_CONNECTIONS )
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
