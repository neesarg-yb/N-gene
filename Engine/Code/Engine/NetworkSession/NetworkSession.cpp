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

bool OnJoinRequest( NetworkMessage const &msg, NetworkSender &from ) 
{
	// Fetch the networkID string
	char joinerNetworkID[ MAX_NETWORK_ID_LENGTH + 1 ] = "";
	msg.ReadString( joinerNetworkID, MAX_NETWORK_ID_LENGTH );
	joinerNetworkID[ MAX_NETWORK_ID_LENGTH ] = '\0';

	return from.session.ProcessJoinRequest( joinerNetworkID, from.address );
}

bool OnJoinDeny( NetworkMessage const &msg, NetworkSender &from ) 
{
	eNetworkSessionError error = NUM_NET_SESSION_ERRORS;
	msg.ReadBytes( &error, sizeof(eNetworkSessionError) );

	return from.session.ProcessJoinDeny( error, from.address );
}

bool OnAccept( NetworkMessage const &msg, NetworkSender &from ) 
{
	uint8_t connectionIdx = MAX_SESSION_CONNECTIONS;
	msg.ReadBytes( &connectionIdx, sizeof(uint8_t) );

	return from.session.ProcessJoinAccept( connectionIdx, from.address );
}

bool OnJoinFinished( NetworkMessage const &msg, NetworkSender &from ) 
{
	UNUSED( msg );

	return from.session.ProcessJoinFinished( from.address );
}

bool OnUpdateConnection( NetworkMessage const &msg, NetworkSender &from ) 
{
	eNetworkConnectionState updatedState = NET_CONNECTION_DISCONNECTED;
	msg.ReadBytes( &updatedState, sizeof(eNetworkConnectionState) );

	return from.session.ProcessUpdateConnectionState( updatedState, from.address );
}

NetworkSession::NetworkSession( Renderer *currentRenderer /* = nullptr */ )
	: m_theRenderer( currentRenderer )
{
	SetBoundConnectionsToNull();

	// For UI
	m_uiCamera = new Camera();

	// Setting up the Camera
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC

	if( currentRenderer != nullptr )
		m_fonts = currentRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	RegisterCoreMessages();

	// Timers
	m_joinRequestTimer.SetTimer( m_joinTimerSeconds );
}

NetworkSession::~NetworkSession()
{
	// Delete all connections
	for( size_t i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		// If nullptr, skip
		if( m_boundConnections[i] == nullptr )
			continue;

		// Delete
		delete m_boundConnections[i];
		m_boundConnections[i] = nullptr;
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

void NetworkSession::Update()
{
	RemoveDisconnectedConnections();

	ProcessIncoming();

	switch (m_state)
	{
	case NET_SESSION_DISCONNECTED:
		UpdateSessionDisconnected();
		break;

	case NET_SESSION_BOUND:
		UpdateSessionBound();
		break;

	case NET_SESSION_CONNECTING:
		UpdateSessionConnecting();
		break;

	case NET_SESSION_JOINING:
		UpdateSessionJoining();
		break;

	case NET_SESSION_READY:
		UpdateSessionReady();
		break;

	default:
		break;
	}
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
	std::string	columnTitleStr		= Stringf( "%-4s  %-3s  %-21s  %-12s  %-7s  %-7s  %-7s  %-7s  %-7s  %-7s  %-16s  %-7s", "--", "idx", "address", "simsndrt(hz)", "rtt(s)", "loss(%)", "lrcv(s)", "lsnt(s)", "nsntack", "hrcvack", "rcvbits", "ucnfrmR" );
	m_theRenderer->DrawTextInBox2D( columnTitleStr.c_str(), Vector2( 0.f, 0.5f ), columnTitlesBox, m_uiBodyFontSize, RGBA_KHAKI_COLOR, m_fonts, TEXT_DRAW_OVERRUN );

	// Each Connections
	uint	numOfConnectionDisplayed = 0U;
	Vector2	connectionDetailBoxSize = Vector2( columnTitlesBox.maxs.x - columnTitlesBox.mins.x, columnTitlesBox.maxs.y - columnTitlesBox.mins.y );
	for( int i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] == nullptr )
			continue;

		// To indicate if connection is "host", "local", etc..
		bool isLocal = m_boundConnections[i]->IsMe();
		bool isHost  = m_boundConnections[i]->IsHost();
		std::string connectionLebelStr = "";
		if( isHost )
			connectionLebelStr += "H ";
		if( isLocal )
			connectionLebelStr += "L ";
		
		// idx
		std::string idxStr = std::to_string( i );

		// address
		std::string connectionAddrStr = m_boundConnections[i]->GetAddress().AddressToString();

		// simsndrt(hz)
		std::string simsndrt = Stringf( "%dhz", m_boundConnections[i]->GetCurrentSendFrequency() );

		// rtt(s)
		std::string rttStr = Stringf( "%.3f", m_boundConnections[i]->m_rtt );

		// loss(%)
		std::string lossPercentStr = Stringf( "%.2f", m_boundConnections[i]->m_loss * 100.f );

		// lrcv(s)
		uint64_t lastReceivedDeltaHPC = Clock::GetCurrentHPC() - m_boundConnections[i]->m_lastReceivedTimeHPC;
		double	 lastReceivedDeltaSec = Clock::GetSecondsFromHPC( lastReceivedDeltaHPC );
		std::string lrcvStr = Stringf( "%.3f", lastReceivedDeltaSec);

		// lsnt(s)
		uint64_t lastSentDeltaHPC = Clock::GetCurrentHPC() - m_boundConnections[i]->m_lastSendTimeHPC;
		double	 lastSentDeltaSec = Clock::GetSecondsFromHPC( lastSentDeltaHPC );
		std::string lsntStr = Stringf( "%.3f", lastSentDeltaSec);

		// nsntack
		std::string sntackSrt = Stringf( "%d", m_boundConnections[i]->m_nextSentAck );

		// hrcvack
		std::string rcvackStr = Stringf( "%d", m_boundConnections[i]->m_highestReceivedAck );

		// rcvbits
		std::bitset< 16 > rcvbit = m_boundConnections[i]->m_receivedAcksBitfield;
		std::string rcvbitsStr = rcvbit.to_string();

		// ucnfrmR
		std::string ncnfrm_relStr = Stringf( "%d", m_boundConnections[i]->GetUnconfirmedSendReliablesCount() );

		// Calculate the AABB
		Vector2	mins = Vector2( columnTitlesBox.mins.x, columnTitlesBox.mins.y - ( ++numOfConnectionDisplayed * (m_uiBodyFontSize * 1.1f) ) );
		AABB2 connectionDetailBox = AABB2( mins, mins + connectionDetailBoxSize );

		// Draw the string
		std::string	connectionRowStr = Stringf( "%-4s  %-3s  %-21s  %-12s  %-7s  %-7s  %-7s  %-7s  %-7s  %-7s  %-16s  %-7s", connectionLebelStr.c_str(), idxStr.c_str(), connectionAddrStr.c_str(), simsndrt.c_str(),rttStr.c_str(), lossPercentStr.c_str(), lrcvStr.c_str(), lsntStr.c_str(), sntackSrt.c_str(), rcvackStr.c_str(), rcvbitsStr.c_str(), ncnfrm_relStr.c_str() );
		m_theRenderer->DrawTextInBox2D( connectionRowStr.c_str(), Vector2( 0.f, 0.5f ), connectionDetailBox, m_uiBodyFontSize, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_OVERRUN );
	}
}

void NetworkSession::UpdateSessionDisconnected()
{

}

void NetworkSession::UpdateSessionBound()
{

}

void NetworkSession::UpdateSessionConnecting()
{
	if( m_myConnection->GetState() == NET_CONNECTION_CONNECTED )
	{
		UpdateStateTo( NET_SESSION_JOINING );
		return;
	}

	// Not connected => send JOIN_REQUEST after some interval
	if( m_joinRequestTimer.CheckAndReset() )
	{
		NetworkMessage msg( "join_request", LITTLE_ENDIAN );
		msg.WriteString( m_myConnection->GetNetworkID().c_str() );

		m_hostConnection->Send( msg );
	}

	// Timeout check
	if( m_joinTimeoutTimer.CheckAndReset() )
	{
		SetError( NET_SESSION_ERROR_TIMEOUT, "Host didn't respond. Timed out!" );
		UpdateStateTo( NET_SESSION_DISCONNECTED );
	}
}

void NetworkSession::UpdateSessionJoining()
{
	// Does nothing
	//	Just waits until my connection is marked as ready,
	//	Which happens in ProcessJoinFinished(), called on NET_MESSAGE_JOIN_FINISHED
}

void NetworkSession::UpdateSessionReady()
{

}

void NetworkSession::UpdateStateTo( eNetworkSessionState newState )
{
	// Reset timers
	if( m_state != newState )
	{
		if( newState == NET_SESSION_DISCONNECTED )
			m_joinTimeoutTimer.Reset();
	}

	// Change states
	m_state = newState;
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
	{
		m_mySocket->EnableNonBlocking();
		return true;
	}

	// on failure, return after deleting the socket
	delete m_mySocket;
	m_mySocket = nullptr;
	return false;
}

void NetworkSession::RegisterCoreMessages()
{
	RegisterNetworkMessage( NET_MESSAGE_PING,						"ping",					OnPing,				NET_MESSAGE_OPTION_CONNECTIONLESS );
	RegisterNetworkMessage( NET_MESSAGE_PONG,						"pong",					OnPong,				NET_MESSAGE_OPTION_CONNECTIONLESS );
	RegisterNetworkMessage( NET_MESSAGE_HEARTBEAT,					"heartbeat",			OnHeartbeat,		NET_MESSAGE_OPTION_REQUIRES_CONNECTION );
	
	// Connection Management
	RegisterNetworkMessage( NET_MESSAGE_JOIN_REQUEST,				"join_request",			OnJoinRequest,		NET_MESSAGE_OPTION_CONNECTIONLESS );
	RegisterNetworkMessage( NET_MESSAGE_JOIN_DENY,					"join_deny",			OnJoinDeny,			NET_MESSAGE_OPTION_REQUIRES_CONNECTION );
	RegisterNetworkMessage( NET_MESSAGE_JOIN_ACCEPT,				"join_accept",			OnAccept,			NET_MESSAGE_OPTION_RELIABLE_IN_ORDER );
	RegisterNetworkMessage( NET_MESSAGE_JOIN_FINISHED,				"join_finished",		OnJoinFinished,		NET_MESSAGE_OPTION_RELIABLE_IN_ORDER );
	RegisterNetworkMessage( NET_MESSAGE_UPDATE_CONNECTION_STATE,	"update_connection",	OnUpdateConnection,	NET_MESSAGE_OPTION_RELIABLE_IN_ORDER );
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
		if( m_boundConnections[i] == nullptr )
			continue;

		m_boundConnections[i]->FlushMessages();
	}
}

void NetworkSession::Host( char const *myID, uint16_t port, uint16_t portRange /*= DEFAULT_PORT_RANGE */ )
{
	if( IsRunning() )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Can't host.. the session is already running!" );
		return;
	}

	bool portBound = BindPort( port, portRange );
	if( portBound == false )
	{
		SetError( NET_SESSION_ERROR_INTERNAL, "Can't bind the port..!" );
		return;
	}

	NetworkConnection *myConnectionAsHost = new NetworkConnection( 0, m_mySocket->m_address, myID, *this );
	DeleteConnection( m_myConnection );	// Make sure the old my connection is deleted

	// Convenience pointers
	m_myConnection		= myConnectionAsHost;
	m_hostConnection	= myConnectionAsHost;

	// Connection lists
	m_allConnections.push_back( myConnectionAsHost );
	m_boundConnections[ 0 ] = myConnectionAsHost;

	// Connection: ready
	myConnectionAsHost->UpdateStateTo( NET_CONNECTION_READY );
	
	// Session: ready
	UpdateStateTo( NET_SESSION_READY );
}

void NetworkSession::Join( char const *myID, NetworkAddress const &hostAddress )
{
	bool portBound = BindPort( hostAddress.port, MAX_SESSION_CONNECTIONS );
	if( portBound == false )
	{
		SetError( NET_SESSION_ERROR_INTERNAL, "Can't bind the port..!" );
		return;
	}

	// Make a connection for the host
	NetworkConnection *host = new NetworkConnection( 0, hostAddress, "host", *this );
	
	DeleteConnection( m_hostConnection );
	m_hostConnection = host;

	m_allConnections.push_back( host );
	m_boundConnections[0] = host;
	
	host->UpdateStateTo( NET_CONNECTION_CONNECTED );

	// Send join request to join
	NetworkMessage joinRequestMessage("join_request", LITTLE_ENDIAN);
	joinRequestMessage.WriteString( myID );
	host->Send( joinRequestMessage );

	// Make a connection for yourself
	NetworkConnection *myConnection = new NetworkConnection( INVALID_INDEX_IN_SESSION, m_mySocket->m_address, myID, *this );
	
	DeleteConnection( m_myConnection );
	m_myConnection = myConnection;

	UpdateStateTo( NET_SESSION_CONNECTING );
}

void NetworkSession::Disconnect()
{
	DeleteAllConnections();
	UpdateStateTo( NET_SESSION_DISCONNECTED );
}

bool NetworkSession::ProcessJoinRequest( char *networkID, NetworkAddress const &reqFromAddress )
{
	eNetworkSessionError joinRequestError = NET_SESSION_OK;

	// Error: I'm not a host
	if( IsHosting() == false )
	{
		joinRequestError = NET_SESSION_ERROR_JOIN_DENIED_NOT_HOST;

		NetworkMessage denyMessage( "join_deny" );
		denyMessage.WriteBytes( sizeof(eNetworkSessionError), &joinRequestError );

		SendDirectMessageTo( denyMessage, reqFromAddress );
		return false;
	}

	// I'm a host!
	// Error: I'm not listening
	if( IsListening() == false )
	{
		joinRequestError = NET_SESSION_ERROR_JOIN_DENIED_CLOSED;

		NetworkMessage denyMessage( "join_deny" );
		denyMessage.WriteBytes( sizeof(eNetworkSessionError), &joinRequestError );

		SendDirectMessageTo( denyMessage, reqFromAddress );
		return false;
	}

	// I'm a host & listening
	// Error: Lobby is full
	if( IsLobbyFull() == true )
	{
		joinRequestError = NET_SESSION_ERROR_JOIN_DENIED_FULL;

		NetworkMessage denyMessage( "join_deny" );
		denyMessage.WriteBytes( sizeof(eNetworkSessionError), &joinRequestError );

		SendDirectMessageTo( denyMessage, reqFromAddress );
		return false;
	}

	if( ConnectionAlreadyExists( reqFromAddress ) == true )
		return true;

	// Bind as new connection
	int idx = GetIndexForNewConnection();
	NetworkConnection *newConnection = new NetworkConnection( idx, reqFromAddress, networkID, *this );
	
	m_allConnections.push_back( newConnection );
	m_boundConnections[ idx ] = newConnection;

	// Send: JOIN_ACCEPT
	NetworkMessage acceptMessage( "join_accept" );
	acceptMessage.WriteString( m_hostConnection->GetNetworkID().c_str() );
	acceptMessage.WriteBytes( sizeof(int), &idx );
	newConnection->Send( acceptMessage );

	// Send: JOIN_FINISHED
	NetworkMessage joinFinishedMessage( "join_finished" );
	newConnection->Send( joinFinishedMessage );

	return true;
}

bool NetworkSession::ProcessJoinDeny( eNetworkSessionError errorCode, NetworkAddress const &senderAddress )
{
	if( (errorCode == NUM_NET_SESSION_ERRORS) || (senderAddress != m_hostConnection->GetAddress()) )
		return false;

	SetError( errorCode, "Join requested denied from the host!" );

	// Connection, disconnected
	m_myConnection->UpdateStateTo( NET_CONNECTION_DISCONNECTED );

	// Session, disconnected
	UpdateStateTo( NET_SESSION_DISCONNECTED );

	return true;
}

bool NetworkSession::ProcessJoinAccept( uint8_t connectionIdx, NetworkAddress const &senderAddress )
{
	if( (connectionIdx == MAX_SESSION_CONNECTIONS) || (senderAddress != m_hostConnection->GetAddress()) )
		return false;

	BindConnection( connectionIdx, m_myConnection );
	m_myConnection->UpdateStateTo( NET_CONNECTION_CONNECTED );

	return false;
}

bool NetworkSession::ProcessJoinFinished( NetworkAddress const &senderAddress )
{
	if( m_hostConnection->GetAddress() == senderAddress )
	{
		UpdateStateTo( NET_SESSION_READY );
		return true;
	}
	else
		return false;
}

bool NetworkSession::ProcessUpdateConnectionState( eNetworkConnectionState state, NetworkAddress const &senderAddress )
{
	NetworkConnection *connection = GetConnection( senderAddress );

	if( connection == nullptr )
		return false;

	connection->UpdateStateTo( state );
	return true;
}

void NetworkSession::SetError( eNetworkSessionError error, char const *str )
{
	m_errorCode		= error;
	m_errorString	= str;
}

void NetworkSession::ClearError()
{
	m_errorCode		= NET_SESSION_OK;
	m_errorString	= "";
}

eNetworkSessionError NetworkSession::GetLastError( std::string *outStr )
{
	*outStr = m_errorString;
	eNetworkSessionError errorCpy = m_errorCode;
	
	ClearError();
	
	return errorCpy;
}

bool NetworkSession::IsLobbyFull() const
{
	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] == nullptr )
			return false;
	}

	return true;
}

void NetworkSession::SendPacket( NetworkPacket &packetToSend )
{
	uint8_t idx = packetToSend.m_header.connectionIndex;
	packetToSend.m_header.connectionIndex = GetMyConnectionIndex();
	packetToSend.WriteHeader( packetToSend.m_header );

	m_mySocket->SendTo( m_boundConnections[idx]->GetAddress(), packetToSend.GetBuffer(), packetToSend.GetWrittenByteCount() );
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

void NetworkSession::BroadcastMessage( NetworkMessage &messageToBroadcast, NetworkConnection const *excludeConnection /* = nullptr */ )
{
	for each (NetworkConnection* connection in m_boundConnections)
	{
		if( connection != nullptr )
		{
			bool exclude = false;

			// Exclude if it is same connection..
			if( excludeConnection != nullptr )
				exclude = (*connection == *excludeConnection);

			if( exclude == false )
				connection->Send( messageToBroadcast );
		}
	}
}

NetworkConnection* NetworkSession::CreateConnection( NetworkConnectionInfo const &info )
{
	NetworkConnection *newConnection = new NetworkConnection( info, *this );
	m_allConnections.push_back( newConnection );

	uint8_t indexInSession = (uint8_t)newConnection->GetIndexInSession();
	if( indexInSession != INVALID_INDEX_IN_SESSION )
		BindConnection( indexInSession, newConnection );

	return newConnection;
}

void NetworkSession::DeleteConnection( NetworkConnection* connection )
{
	// If it's the convenience pointer, set it to nullptr
	if( connection == m_hostConnection )
		m_hostConnection = nullptr;
	if( connection == m_myConnection )
		m_myConnection = nullptr;

	// Delete it from the all Connections vector
	bool connectionFound = false;
	for( uint i = 0; i < m_allConnections.size(); i++ )
	{
		if( m_allConnections[i] != connection )
			continue;
		
		// It is the connection we're looking for
		connectionFound = true;

		// Fast delete
		std::swap( m_allConnections[i], m_allConnections.back() );
		delete m_allConnections.back();
		m_allConnections.back() = nullptr;

		m_allConnections.pop_back();

		break;
	}

	// It was never registered, return
	if( connectionFound == false )
	{
		delete connection;
		return;
	}

	// Remove it from bound connections
	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] != connection )
			continue;

		// Just remove the reference, we deleted it from all connection
		m_boundConnections[i] = nullptr;
	}
}

void NetworkSession::BindConnection( uint8_t idx, NetworkConnection *connection )
{
	// Update its index
	connection->m_info.indexInSession = idx;

	// Delete if a connection is already there
	if( m_boundConnections[idx] != nullptr )
	{
		delete m_boundConnections[idx];
		m_boundConnections[idx] = nullptr;
	}

	// Bind the new one!
	m_boundConnections[idx] = connection;
}

void NetworkSession::DeleteAllConnections()
{
	for( int i = 0; i < m_allConnections.size(); i++ )
		DeleteConnection( m_allConnections[i] );
}

void NetworkSession::RemoveDisconnectedConnections()
{
	for( int i = 0; i < m_allConnections.size(); i++ )
	{
		NetworkConnection	&thisConnection = *m_allConnections[i];
		bool				 isDisconnected = (thisConnection.GetState() == NET_CONNECTION_DISCONNECTED);
		
		if( isDisconnected == false )
			continue;

		// This connection marked itself as disconnected..
		bool isMyConnection		= (thisConnection == *m_myConnection);
		bool isHostConnection	= (thisConnection == *m_hostConnection);
		if( isMyConnection || isHostConnection )
		{
			// If I or host gets disconnected, destroy all connections
			DeleteAllConnections();

			// Set session state to disconnected
			UpdateStateTo( NET_SESSION_DISCONNECTED );
		}
		else
			DeleteConnection( &thisConnection );
	}
}

void NetworkSession::SetBoundConnectionsToNull()
{
	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		NetworkConnection* &connection = m_boundConnections[i];

		if( connection != nullptr )
			connection = nullptr;
	}
}

bool NetworkSession::ConnectionAlreadyExists( NetworkAddress const &address )
{
	if( m_myConnection->GetAddress() == address )
		return true;

	if( m_hostConnection->GetAddress() == address )
		return true;

	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] == nullptr )
			continue;

		if( m_boundConnections[i]->GetAddress() == address )
			return true;
	}

	return false;
}

int NetworkSession::GetIndexForNewConnection() const
{
	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] == nullptr )
			return i;
	}

	return -1;
}

uint8_t NetworkSession::GetMyConnectionIndex() const
{
	// defaults to INVALID index
	uint8_t idx = 0xff;

	for( uint i = 0; i < MAX_SESSION_CONNECTIONS; i++ )
	{
		if( m_boundConnections[i] == nullptr )
			continue;

		// If connection found..
		if( m_boundConnections[i]->GetAddress() == m_mySocket->m_address )
		{
			// If it is under max indices allowed
			if( i < 0xff )
				idx = (uint8_t)i;

			break;
		}
	}

	return idx;
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	if( idx < 0 || idx >= MAX_SESSION_CONNECTIONS )
		return nullptr;
	else
		return m_boundConnections[idx];
}

NetworkConnection* NetworkSession::GetConnection( NetworkAddress const &address )
{
	NetworkConnection *connection = nullptr;

	for each (NetworkConnection* boundConnection in m_boundConnections)
	{
		if( boundConnection == nullptr )
			continue;

		if( boundConnection->GetAddress() == address )
		{
			connection = boundConnection;
			break;
		}
	}

	return connection;
}

bool NetworkSession::IsRegistered( NetworkConnection const *connection ) const
{
	for each (NetworkConnection const *client in m_allConnections)
	{
		if( *connection == *client )
			return true;
	}

	return false;
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
	for( uint8_t index = NUM_CORE_NET_SESSION_MESSAGES; index <= 0xff; index++ )
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
	for each( NetworkConnection* connection in m_boundConnections )
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
			NetworkConnection *receivedForConnection = m_boundConnections[ receivedConnIdx ];
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
					thisSender.connection = m_boundConnections[ packet->m_header.connectionIndex ];		// If sender has a valid connection, fill it in

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

// NetworkConnection* NetworkSession::AddConnection( int idx, NetworkAddress &addr )
// {
// 		// If idx is not in range
// 		if( idx < 0 || idx >= MAX_SESSION_CONNECTIONS )
// 			return nullptr;
// 
// 		// If there's a connection there already, delete it
// 		if( m_boundConnections[idx] != nullptr )
// 		{
// 			delete m_boundConnections[idx];
// 			m_boundConnections[idx] = nullptr;
// 		}
// 
// 		// Set new connection to that index
// 		NetworkConnection* thisConnection = new NetworkConnection( idx, addr, *this );
// 		m_boundConnections[idx] = thisConnection;
// 
// 		return m_boundConnections[idx];
// }
