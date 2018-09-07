#pragma once
#include "RemoteCommandService.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

RemoteCommandService::RemoteCommandService( Renderer *currentRenderer /* = nullptr */, uint16_t port /* = 29283 */ )
	: m_defaultPortToHost( port )
	, m_theRenderer( currentRenderer )
{
	m_uiCamera = new Camera();

	// Setting up the Camera
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC

	if( currentRenderer != nullptr )
		m_fonts = currentRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
}

RemoteCommandService::~RemoteCommandService()
{

}

void RemoteCommandService::Update( float deltaSeconds )
{
	switch ( m_currentState )
	{
	case RCS_STATE_INITIAL:
		Update_Initial( deltaSeconds );
		break;

	case RCS_STATE_HOSTING:
		Update_Hosting( deltaSeconds );
		break;

	case RCS_STATE_CLIENT:
		Update_Client( deltaSeconds );
		break;

	default:
		GUARANTEE_RECOVERABLE( false, "RemoteCommandService: Invalid Update State..!" );
		break;
	}
}

void RemoteCommandService::Render() const
{
	m_theRenderer->BindCamera( m_uiCamera );

	// To form an overlay: do not clear screen, make depth of every pixel 1.f, do not write new depth..
	m_theRenderer->UseShader( nullptr );
	m_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	// Draw overlay
	AABB2 backgroundBox = m_screenBounds.GetBoundsFromPercentage( Vector2( 0.8f, 0.8f ), Vector2( 1.f, 1.f ) );
	m_theRenderer->DrawAABB( backgroundBox, m_uiBackgroundColor );

	AABB2 infoBox		= backgroundBox.GetBoundsFromPercentage( Vector2( 0.01f, 0.9f ), Vector2( 0.99f, 1.0f ) );
	AABB2 myAddressBox	= backgroundBox.GetBoundsFromPercentage( Vector2( 0.01f, 0.8f ), Vector2( 0.99f, 0.9f ) );
	AABB2 clientListBox = backgroundBox.GetBoundsFromPercentage( Vector2( 0.01f, 0.0f ), Vector2( 0.99f, 0.7f ) );
	
	std::string connectionTypeStr;
	switch (m_currentState)
	{
	case RCS_STATE_HOSTING:
		connectionTypeStr = "Hosting";
		break;
	case RCS_STATE_CLIENT:
		connectionTypeStr = "Client";
		break;
	default:
		connectionTypeStr = "Not Connected";
		break;
	}
	std::string myAddressString = "...";
	if( m_hostSocket != nullptr )
		myAddressString = m_hostSocket->m_address.IPToString() + ":" + m_hostSocket->m_address.PortToString();

	myAddressString += Stringf( " [%s]", connectionTypeStr.c_str() );
	m_theRenderer->DrawTextInBox2D( "Remote Command Service:",	Vector2(0.0f, 0.5f), infoBox,		0.030f, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
	m_theRenderer->DrawTextInBox2D( myAddressString.c_str(),	Vector2(1.0f, 0.5f), myAddressBox,	0.025f, RGBA_GREEN_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );

	std::string clientConnectionsString = "Connected Clients:";
	for ( int i = 0; i < m_clientSockets.size(); i++ )
	{
		std::string clientAddressStr = Stringf("\n (%d) ", i) + m_clientSockets[i]->m_address.IPToString() + ":" + m_clientSockets[i]->m_address.PortToString();
		clientConnectionsString		+= clientAddressStr;
	}

	m_theRenderer->DrawTextInBox2D( clientConnectionsString.c_str(), Vector2(0.f, 1.f), clientListBox, 0.025f, RGBA_WHITE_COLOR, m_fonts, TEXT_DRAW_OVERRUN );
}

bool RemoteCommandService::ConnectToHost( NetworkAddress const &hostAddress )
{
	// If connected to different host, Start from fresh! 
	if( m_hostSocket != nullptr )
		ClearHostData();

	m_hostSocket = new TCPSocket();
	
	bool connectedToHost = m_hostSocket->Connect( hostAddress );
	if( !connectedToHost )
	{
		delete m_hostSocket;
		m_hostSocket = nullptr;

		ConsolePrintf( "RCS: Couldn't connect to host %s", hostAddress.IPToString().c_str() );
	}
	else
		ConsolePrintf( "RCS: Connected to host %s", hostAddress.IPToString().c_str() );

	return connectedToHost;
}

void RemoteCommandService::SendMessageToClient( uint idx, bool isEcho, char const *msg )
{
	// Get the client socket
	TCPSocket *socket = GetSocketAtIndex( idx );
	if( socket == nullptr )
		return;

	SendMessageUsingSocket( *socket, isEcho, msg );
}

void RemoteCommandService::SendMessageUsingSocket( TCPSocket &endSocket, bool isEcho, char const *msg )
{
	BytePacker message( BIG_ENDIAN );

	message.WriteBytes( 1, &isEcho );
	message.WriteString( msg );

	size_t length = message.GetWritableByteCount();
	GUARANTEE_RECOVERABLE( length <= 0xffff, "RCS Error: Format doesn't support length larger than unsigned short!" );

	uint16_t usLength = (uint16_t)length;
	ChangeEndiannessTo( sizeof(usLength), &usLength, BIG_ENDIAN );

	endSocket.Send( &usLength, sizeof(usLength) );
	endSocket.Send( message.GetBuffer(), length );
}

void RemoteCommandService::Update_Initial( float deltaSeconds )
{
	NetworkAddress localHostAddress	= NetworkAddress::GetLocal();
	localHostAddress.port			= m_defaultPortToHost;

	// TODO: Trying to connect to host every frame is NOT EFFICIENT!
	if( ConnectToHost( localHostAddress ) )
	{
		m_currentState = RCS_STATE_CLIENT;
		ConsolePrintf( "Is Client!" );
	}
	else
		TryHosting( deltaSeconds );
}

void RemoteCommandService::Update_Hosting( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Service every client connections
	ServiceClientConnections();

	// Accept new connections
	TCPSocket *newClient = m_hostSocket->Accept();
	if( newClient == nullptr )
		return;

	// Create a new bytepacker for every new connection
	BytePacker *clientsBytePacker = new BytePacker( BIG_ENDIAN );

	// Store client & its bytepacker
	m_clientSockets.push_back( newClient );
	m_bytePackers.push_back( clientsBytePacker );

	// Make client socket, non-blocking
	newClient->EnableNonBlocking();
}

void RemoteCommandService::Update_Client( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void RemoteCommandService::TryHosting( float deltaSeconds )
{
	// Assign the host socket..
	if( m_hostSocket == nullptr )
	{
		// Start hosting..
		m_hostSocket		= new TCPSocket();
		bool isListening	= m_hostSocket->Listen( m_defaultPortToHost, 16U );

		// Success: Transition to hosting state..
		if( isListening )
		{
			m_currentState = RCS_STATE_HOSTING;
			bool isNonBlocking = m_hostSocket->EnableNonBlocking();
			GUARANTEE_RECOVERABLE( isNonBlocking, "RCS: Couldn't make the hosting socket non-blocking!" );
			ConsolePrintf( "Is Hosting..! Non-Blocking = %s", (isNonBlocking) ? "YES" : "NO" );
			return;
		}
		else														// Failed to start listening
		{
			// Don't do anything for few seconds..
			m_retryHostingTimeElapased = 0.f;
			ConsolePrintf( "RCS: Failed to start hosting locally! Retrying in %d seconds..", m_retryHostingInSeconds );
		}
	}

	// We already have a host socket
	if( m_retryHostingTimeElapased <= m_retryHostingInSeconds )		// It is not time to retry hosting..
	{
		// Wait until it's time to retry
		m_retryHostingTimeElapased += deltaSeconds;
		return;
	}
	else															// Attempt hosting, enough time has been elapsed
	{
		bool isListening = m_hostSocket->Listen( m_defaultPortToHost, 16U );
		if( isListening )
			m_currentState = RCS_STATE_HOSTING;

		// If failed, let's wait for more
		m_retryHostingTimeElapased = 0.f;
	}
}

void RemoteCommandService::ClearHostData()
{
	// Deletes the host socket
	if( m_hostSocket == nullptr )
		return;
	else
	{
		delete m_hostSocket;
		m_hostSocket = nullptr;
	}

	// Deletes all the clients' data
	while( m_clientSockets.size() > 0 )
	{
		// Delete client socket
		delete m_clientSockets[0];
		m_clientSockets[0] = nullptr;

		// Fast remove
		uint lastIndex = (uint)m_clientSockets.size() - 1U;
		std::swap( m_clientSockets[0], m_clientSockets[lastIndex] );
		m_clientSockets.pop_back();

		// Delete its bytepacker
		delete m_bytePackers[0];
		m_bytePackers[0] = nullptr;

		std::swap( m_bytePackers[0], m_bytePackers[lastIndex] );
		m_bytePackers.pop_back();
	}
}

TCPSocket* RemoteCommandService::GetSocketAtIndex( uint idx )
{
	if( idx >= m_clientSockets.size() )
		return nullptr;
	else
		return m_clientSockets[ idx ];
}

void RemoteCommandService::ServiceClientConnections()
{
	for( int i = 0; i < m_clientSockets.size(); i++ )
		PopulateByteBufferForClient( i );
}

void RemoteCommandService::PopulateByteBufferForClient( uint clientIdx )
{
	// Get client's socket & bytepacker
	TCPSocket *clientSocket = GetSocketAtIndex( clientIdx );
	if( clientSocket == nullptr )
		return;

	BytePacker *clientBytePacker = m_bytePackers[ clientIdx ];

	if( clientBytePacker->GetWrittenByteCount() < 2 )
	{
		byte_t	lengthOfCommand[2];
		size_t	bytesToReadLength = 2 - clientBytePacker->GetWrittenByteCount();
		int		read = clientSocket->Receive( lengthOfCommand, bytesToReadLength );
		if( read > 0 )
			clientBytePacker->WriteBytes( read, lengthOfCommand, false );
	}

	bool isReadyToProcess = false;
	if( clientBytePacker->GetWrittenByteCount() >= 2 )
	{
		// Get total length of command, we're expecting to read as one whole instruction!
		uint16_t lengthInBigEndian = ( (uint16_t *)clientBytePacker->GetBuffer() )[0];
		ChangeEndiannessFrom( sizeof(uint16_t), &lengthInBigEndian, BIG_ENDIAN );
		uint16_t lengthOfCommand = lengthInBigEndian;

		uint bytesNeeded = (lengthOfCommand + 2U) - (uint)clientBytePacker->GetWrittenByteCount();
		if( bytesNeeded > 0 )
		{
			byte_t tempBuffer[ 256 ];
			int read = clientSocket->Receive( tempBuffer, 256 );
			
			clientBytePacker->WriteBytes( read, tempBuffer, false );
			bytesNeeded -= read;
		}

		isReadyToProcess = ( bytesNeeded == 0 );
	}

	if( isReadyToProcess )
	{
		uint16_t commandLength;
		clientBytePacker->ReadBytes( &commandLength, 2 );

		bool isEcho;
		clientBytePacker->ReadBytes( &isEcho, 1 );

		char commandString[256];
		clientBytePacker->ReadString( commandString, 256 );

		clientBytePacker->ResetWrite();

		ConsolePrintf( "Received %d bytes long Command: IsEcho = %d; Message = \"%s\".", commandLength, isEcho, commandString );
	}
}
