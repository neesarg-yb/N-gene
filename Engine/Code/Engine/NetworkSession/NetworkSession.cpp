#pragma once
#include "NetworkSession.hpp"
#include <bitset>
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

bool OnPing( NetworkMessage const &msg, NetworkSender &from )
{
	char str[256];
	msg.Read( str, 256 );

	ConsolePrintf( "Received ping from %s => %s", from.address.AddressToString().c_str(), str ); 

	// ping responds with pong
	NetworkMessage pong( "pong" ); 
	if( from.connection != nullptr )
		from.connection->Send( pong );
	else
		from.session.SendDirectMessageTo( pong, from.address );

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnPong( NetworkMessage const &msg, NetworkSender &from )
{
	UNUSED( msg );

	ConsolePrintf( "PONG! Received from %s", from.address.AddressToString().c_str() );
	return false;
}

bool OnHeartbeat( NetworkMessage const &msg, NetworkSender &from )
{
	UNUSED( msg );
	
	if( from.connection == nullptr )
		return false;
	else
	{
		// ConsolePrintf( "Heartbeat Received from [%d] connection.", from.connection->m_indexInSession );
		return true;
	}
}

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
	
	RegisterCoreMessages();
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

	// Delete all message definitions
	for( int i = 0; i < 256; i++ )
	{
		if( m_registeredMessages[i] == nullptr )
			continue;

		delete m_registeredMessages[i];
		m_registeredMessages[i] = nullptr;
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
	AABB2 backgroundBox = m_screenBounds.GetBoundsFromPercentage( Vector2( 0.f, 0.8f ), Vector2( 1.f, 1.f ) );
	m_theRenderer->DrawAABB( backgroundBox, m_uiBackgroundColor );

	// Title Box
	AABB2		titleBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.f, 0.9f ), Vector2( 1.f, 1.f ) );
	std::string	titleStr = "NETWORK SESSION";
	m_theRenderer->DrawTextInBox2D( titleStr.c_str(), Vector2( 0.f, 0.5f ), titleBox, m_uiTitleFontSize, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// Simulated Rate, Lag & Loss
	std::string sendRateStr			= Stringf( "%dhz", m_simulatedSendFrequency );
	std::string lossPercentageStr	= Stringf( "%.2f%%", m_simulatedLossFraction * 100.f );
	std::string simLagRangeStr		= Stringf( "%dms - %dms", m_simulatedMinLatency_ms, m_simulatedMaxLatency_ms );
	std::string heartbeatHzStr		= Stringf( "%.2fhz", m_heartbeatFrequency );
	AABB2		srllBox				= backgroundBox.GetBoundsFromPercentage( Vector2( 0.1f, 0.6f ), Vector2( 1.f, 0.9f ) );
	std::string srllStr				= Stringf( "%-8s: %s (%s: %s)\n%-8s: %s\n%-8s: %s", "rate", sendRateStr.c_str(), "heartbeat", heartbeatHzStr.c_str(), "sim_lag", simLagRangeStr.c_str(), "sim_loss", lossPercentageStr.c_str() );
	m_theRenderer->DrawTextInBox2D( srllStr.c_str(), Vector2( 0.f, 1.f ), srllBox, m_uiBodyFontSize, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// My Socket Address
	AABB2		myAddressBaseBox	= backgroundBox.GetBoundsFromPercentage   ( Vector2( 0.0f, 0.5f ), Vector2( 1.f, 0.7f ) );
	AABB2		myAddressTitleBox	= myAddressBaseBox.GetBoundsFromPercentage( Vector2( 0.0f, 0.5f ), Vector2( 1.f, 1.0f ) );
	AABB2		myAddressBox		= myAddressBaseBox.GetBoundsFromPercentage( Vector2( 0.1f, 0.0f ), Vector2( 1.f, 0.5f ) );
	std::string myAddressTitle		= "My Socket Address:";
	std::string socketAddrStr		= m_mySocket->m_address.AddressToString();
	m_theRenderer->DrawTextInBox2D( myAddressTitle.c_str(), Vector2( 0.f, 0.5f ), myAddressTitleBox, m_uiBodyFontSize, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
	m_theRenderer->DrawTextInBox2D( socketAddrStr.c_str(),  Vector2( 0.f, 0.5f ), myAddressBox,      m_uiBodyFontSize, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// Connections Heading
	AABB2		connectionsHeadingBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.0f, 0.4f ), Vector2( 1.f, 0.5f ) );
	std::string connectionsHeadingStr = "Connections:";
	m_theRenderer->DrawTextInBox2D( connectionsHeadingStr.c_str(), Vector2( 0.f, 0.5f ), connectionsHeadingBox, m_uiBodyFontSize, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	// Title Column of Table: All Connections
	AABB2		allConnectionsBox	= backgroundBox.GetBoundsFromPercentage    ( Vector2( 0.1f, 0.f ), Vector2( 1.f, 0.4f ) );
	AABB2		columnTitlesBox		= allConnectionsBox.GetBoundsFromPercentage( Vector2( 0.f, 0.9f ), Vector2( 1.f, 1.0f ) );
	std::string	columnTitleStr		= Stringf( "%-2s  %-3s  %-21s  %-12s  %-7s  %-7s  %-7s  %-7s  %-7s  %-7s  %-16s  %-7s", "--", "idx", "address", "simsndrt(hz)", "rtt(s)", "loss(%)", "lrcv(s)", "lsnt(s)", "nsntack", "hrcvack", "rcvbits", "ucnfrmR" );
	m_theRenderer->DrawTextInBox2D( columnTitleStr.c_str(), Vector2( 0.f, 0.5f ), columnTitlesBox, m_uiBodyFontSize, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_OVERRUN );

	// Each Connections
	uint	numOfConnectionDisplayed = 0U;
	Vector2	connectionDetailBoxSize = Vector2( columnTitlesBox.maxs.x - columnTitlesBox.mins.x, columnTitlesBox.maxs.y - columnTitlesBox.mins.y );
	for( int i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_connections[i] == nullptr )
			continue;

		// To indicate which connection is mine!
		int			myIndex		= GetMyConnectionIndex();
		std::string isLocalStr	= (i == myIndex) ? "L" : " ";
		
		// idx
		std::string idxStr = std::to_string( i );

		// address
		std::string connectionAddrStr = m_connections[i]->m_address.AddressToString();

		// simsndrt(hz)
		std::string simsndrt = Stringf( "%dhz", m_connections[i]->GetCurrentSendFrequency() );

		// rtt(s)
		std::string rttStr = Stringf( "%.3f", m_connections[i]->m_rtt );

		// loss(%)
		std::string lossPercentStr = Stringf( "%.2f", m_connections[i]->m_loss * 100.f );

		// lrcv(s)
		uint64_t lastReceivedDeltaHPC = Clock::GetCurrentHPC() - m_connections[i]->m_lastReceivedTimeHPC;
		double	 lastReceivedDeltaSec = Clock::GetSecondsFromHPC( lastReceivedDeltaHPC );
		std::string lrcvStr = Stringf( "%.3f", lastReceivedDeltaSec);

		// lsnt(s)
		uint64_t lastSentDeltaHPC = Clock::GetCurrentHPC() - m_connections[i]->m_lastSendTimeHPC;
		double	 lastSentDeltaSec = Clock::GetSecondsFromHPC( lastSentDeltaHPC );
		std::string lsntStr = Stringf( "%.3f", lastSentDeltaSec);

		// nsntack
		std::string sntackSrt = Stringf( "%d", m_connections[i]->m_nextSentAck );

		// hrcvack
		std::string rcvackStr = Stringf( "%d", m_connections[i]->m_highestReceivedAck );

		// rcvbits
		std::bitset< 16 > rcvbit = m_connections[i]->m_receivedAcksBitfield;
		std::string rcvbitsStr = rcvbit.to_string();

		// ucnfrmR
		std::string ncnfrm_relStr = Stringf( "%d", m_connections[i]->GetUnconfirmedSendReliablesCount() );

		// Calculate the AABB
		Vector2	mins = Vector2( columnTitlesBox.mins.x, columnTitlesBox.mins.y - ( ++numOfConnectionDisplayed * (m_uiBodyFontSize * 1.1f) ) );
		AABB2 connectionDetailBox = AABB2( mins, mins + connectionDetailBoxSize );

		// Draw the string
		std::string	connectionRowStr = Stringf( "%-2s  %-3s  %-21s  %-12s  %-7s  %-7s  %-7s  %-7s  %-7s  %-7s  %-16s  %-7s", isLocalStr.c_str(), idxStr.c_str(), connectionAddrStr.c_str(), simsndrt.c_str(),rttStr.c_str(), lossPercentStr.c_str(), lrcvStr.c_str(), lsntStr.c_str(), sntackSrt.c_str(), rcvackStr.c_str(), rcvbitsStr.c_str(), ncnfrm_relStr.c_str() );
		m_theRenderer->DrawTextInBox2D( connectionRowStr.c_str(), Vector2( 0.f, 0.5f ), connectionDetailBox, m_uiBodyFontSize, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_OVERRUN );
	}
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

void NetworkSession::RegisterCoreMessages()
{
	RegisterNetworkMessage( NET_MESSAGE_PING,		"ping",		 OnPing,		NET_MESSAGE_OPTION_CONNECTIONLESS );
	RegisterNetworkMessage( NET_MESSAGE_PONG,		"pong",		 OnPong,		NET_MESSAGE_OPTION_CONNECTIONLESS );
	RegisterNetworkMessage( NET_MESSAGE_HEARTBEAT,	"heartbeat", OnHeartbeat,	NET_MESSAGE_OPTION_REQUIRES_CONNECTION );
}

void NetworkSession::ProcessIncoming()
{
	ReceivePacket();
	ProcessReceivedPackets();
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
	// Update the index of this messageToSend
	NetworkMessageDefinition const *msgDef = GetRegisteredMessageDefination( messageToSend.m_name );
	messageToSend.SetDefinition( msgDef );
	
	// Send the Packet
	NetworkPacket packetToSend;
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

NetworkConnection* NetworkSession::GetMyConnection()
{
	uint idx = GetMyConnectionIndex();

	return m_connections[ idx ];
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

void NetworkSession::RegisterNetworkMessage( uint8_t index, char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netmessageOptionsFlag )
{
	NetworkMessageDefinition *newDefinition = new NetworkMessageDefinition( (int)index, messageName, cb, netmessageOptionsFlag );
	
	if( m_registeredMessages[ index ] != nullptr )
	{
		delete m_registeredMessages[ index ];
		m_registeredMessages[ index ] = nullptr;
	}

	m_registeredMessages[ index ] = newDefinition;
}

bool NetworkSession::RegisterNetworkMessage( char const *messageName, networkMessage_cb cb, eNetworkMessageOptions netMessageOptionsFlag )
{
	for( uint8_t index = NUM_NET_MESSAGES; index <= 0xff; index++ )
	{
		if( m_registeredMessages[ index ] != nullptr )
			continue;
		else
		{
			NetworkMessageDefinition *newDefinition = new NetworkMessageDefinition( (int)index, messageName, cb, netMessageOptionsFlag );
			m_registeredMessages[ index ] = newDefinition;

			return true;
		}
	}

	return false;
}

NetworkMessageDefinition const* NetworkSession::GetRegisteredMessageDefination( std::string const &definitionName ) const
{
	for( int i = 0; i < 256; i++ )
	{
		// Skip if nullptr
		if( m_registeredMessages[i] == nullptr )
			continue;

		// Note the index if name matches
		if( m_registeredMessages[i]->name == definitionName )
		{
			return m_registeredMessages[i];
		}
	}

	return nullptr;
}

NetworkMessageDefinition const* NetworkSession::GetRegisteredMessageDefination( int defIndex ) const
{
	return m_registeredMessages[ defIndex ];
}

bool NetworkSession::SetHeartbeatFrequency( float frequencyHz )
{
	// We can set it to zero, b/c we'll be dividing by zero when updating the timer
	if( frequencyHz == 0 )
		return false;

	// Set the frequency
	m_heartbeatFrequency = frequencyHz;

	// Update timer in all the connections
	for each( NetworkConnection* connection in m_connections )
	{
		if( connection == nullptr )
			continue;

		connection->UpdateHeartbeatTimer();
	}

	return true;
}

void NetworkSession::SetSimulationLoss( float lossFraction )
{
	m_simulatedLossFraction = ClampFloat01( lossFraction );
}

void NetworkSession::SetSimulationLatency( uint minAddedLatency_ms, uint maxAddedLatency_ms /*= 0U */ )
{
	m_simulatedMinLatency_ms =  minAddedLatency_ms;
	m_simulatedMaxLatency_ms = (minAddedLatency_ms > maxAddedLatency_ms) ? minAddedLatency_ms : maxAddedLatency_ms;
}

void NetworkSession::SetSimulationSendFrequency( uint8_t frequencyHz )
{
	m_simulatedSendFrequency = frequencyHz;
}

void NetworkSession::ReceivePacket()
{
	NetworkAddress sender;
	size_t receivedBytes = 0U;
	
	do 
	{
		void *buffer = malloc( PACKET_MTU );

		receivedBytes = m_mySocket->ReceiveFrom( &sender, buffer, PACKET_MTU );
		
		NetworkPacket *receivedPacket = new NetworkPacket();
		receivedPacket->WriteBytes( receivedBytes, buffer, false );
		bool discardThisPacket = CheckRandomChance( m_simulatedLossFraction );

		// If it is not an empty packet & if we're not discarding
		if( (receivedBytes > 0) && (discardThisPacket == false) )
			QueuePacketForSimulation( receivedPacket, sender );

		// Free the temp buffer
		free( buffer );

	} while ( receivedBytes > 0U );
}

void NetworkSession::ProcessReceivedPackets()
{
	uint64_t currentHPC = Clock::GetCurrentHPC();

	// Check all packets
	while ( m_receivedPackets.size() > 0 )
	{
		// If the top one is due to process
		if( m_receivedPackets.top().timestampHPC <= currentHPC )
		{
			// Get the top packet
			StampedNetworkPacket thisStampedPacket = m_receivedPackets.top();

			// Process it
			ProccessAndDeletePacket( thisStampedPacket.packet, thisStampedPacket.sender );

			// Remove the top from queue
			m_receivedPackets.pop();
		}
		else
			break;	// Because we don't have the top due at this point
	}
}

void NetworkSession::QueuePacketForSimulation( NetworkPacket *newPacket, NetworkAddress &sender )
{
	StampedNetworkPacket stampedPacket( newPacket, sender );

	// Calculate random latency
	uint	range		= m_simulatedMaxLatency_ms - m_simulatedMinLatency_ms;
	int		randomRange	= GetRandomIntInRange( 0, (int)range );
	uint	latency_ms	= m_simulatedMinLatency_ms + (uint)randomRange;

	// Get timestamp out of it
	uint64_t latency_HPC = Clock::GetHPCFromMilliSeconds( latency_ms );
	uint64_t timestamp	 = Clock::GetCurrentHPC() + latency_HPC;

	// Stamp with the latency
	stampedPacket.timestampHPC = timestamp;

	// Add it to priority queue
	m_receivedPackets.push( stampedPacket );
}

void NetworkSession::ProccessAndDeletePacket( NetworkPacket *&packet, NetworkAddress &sender )
{
	if( packet->IsValid() )
	{
		bool headerReadingSuccess = packet->ReadHeader( packet->m_header );
		GUARANTEE_RECOVERABLE( headerReadingSuccess, "Couldn't read the Packet Header successfully!" );

		// Inform the connection about received packet
		uint8_t receivedConnIdx = packet->m_header.connectionIndex;
		if( receivedConnIdx != 0xff )
		{
			NetworkConnection *receivedForConnection = m_connections[ receivedConnIdx ];
			if( receivedForConnection != nullptr )
				receivedForConnection->OnReceivePacket( packet->m_header );
		}

		// Process each messages
		NetworkMessage receivedMessage;
		for( int i = 0; i < packet->m_header.messageCount; i++ )
		{
			bool messageReadSuccess = packet->ReadMessage( receivedMessage, *this );
			GUARANTEE_RECOVERABLE( messageReadSuccess, "Couldn't read the Network Message successfully!" );

			// To get Message Definition from index
			NetworkMessageDefinition *messageDefinition = m_registeredMessages[ receivedMessage.m_header.networkMessageDefinitionIndex ];

			// If that's a valid definition index
			if( messageDefinition != nullptr )
			{
				// Set the pointer to that definition
				receivedMessage.SetDefinition( messageDefinition );

				// Create a NetworkSender
				NetworkSender thisSender = NetworkSender( *this, sender, nullptr );
				if( receivedConnIdx != 0xff )
					thisSender.connection = m_connections[ packet->m_header.connectionIndex ];		// If sender has a valid connection, fill it in

				// Do a callback!
				if( messageDefinition->RequiresConnection() && thisSender.connection == nullptr )
				{
					// Requires a connection, but don't have this address registered!
					// Log an error
					ConsolePrintf( RGBA_RED_COLOR, "IGNORED \"%s\" MESSAGE: Received from address: %s, but it requires a connection", 
													receivedMessage.m_name.c_str(), 
													thisSender.address.AddressToString().c_str() );
				}
				else
				{
					if( thisSender.connection == nullptr )
					{
						// Do the callback
						receivedMessage.GetDefinition()->callback( receivedMessage, thisSender );
					}
					else
					{
						// Hand it over to the Connection
						thisSender.connection->ProcessReceivedMessage( receivedMessage, thisSender );
					}
				}
			}
			else
				ConsolePrintf( "Received invalid messageDefinition Index: %d", receivedMessage.m_header.networkMessageDefinitionIndex );
		}
	}
	else
		ConsolePrintf( RGBA_KHAKI_COLOR, "Bad Packet Received from %s", sender.AddressToString().c_str() );

	// Delete the packet
	delete packet;
	packet = nullptr;
}
