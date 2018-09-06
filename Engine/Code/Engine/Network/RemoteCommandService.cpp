#pragma once
#include "RemoteCommandService.hpp"

#include "Engine/Core/DevConsole.hpp"

RemoteCommandService::RemoteCommandService( uint16_t port /*= 29283 */ )
	: m_defaultPortToHost( port )
{

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

		ConsolePrintf( "RCS: Couldn't connect to host %s", hostAddress.ToString().c_str() );
	}
	else
		ConsolePrintf( "RCS: Connected to host %s", hostAddress.ToString().c_str() );

	return connectedToHost;
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
			ConsolePrintf( "Is Hosting!" );
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
