#pragma once
#include "TCPSocket.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

TCPSocket::~TCPSocket()
{
	if( m_handle != INVALID_SOCKET )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;
	}
}

bool TCPSocket::Listen( uint16_t port, uint maxQueued )
{
	UNUSED( port );
	UNUSED( maxQueued );

	return false;
}

TCPSocket* TCPSocket::Accept()
{
	return nullptr;
}

bool TCPSocket::Connect( NetworkAddress const &networkAddress )
{
	m_handle = ::socket( AF_INET,			// Address Family: IPv4
						 SOCK_STREAM,		// Type: TCP is stream based packet
						 IPPROTO_TCP );		// Protocol: TCP

	if( m_handle == INVALID_SOCKET )
	{
		ConsolePrintf( "Error: Could not create socket!" );
		return false;
	}

	// sockaddr storage is a sockaddr struct that
	// is large enough to fit any other sockaddr struct
	// sizeof(sockaddr_storage) >= sizeof(any other sockaddr)
	sockaddr_storage	socketAddress;
	size_t				addressLength;
	bool				success = networkAddress.ToSocketAddress( (sockaddr*)&socketAddress, &addressLength );

	GUARANTEE_RECOVERABLE( success, "Couldn't get a sockaddr!!" );

	int result = ::connect( m_handle, (sockaddr*)&socketAddress, (int)addressLength );
	if( result == SOCKET_ERROR )
	{
		ConsolePrintf( "Error: Could not connect!" );
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;
		return false;
	}

	return true;
}

void TCPSocket::Close()
{
	if( m_handle != INVALID_SOCKET )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;
	}
}

size_t TCPSocket::Send( void const *data, size_t const dataByteSize )
{
	int		numBytesSent = ::send( m_handle, (const char*)data, (int)dataByteSize, 0 );

	return (numBytesSent != SOCKET_ERROR) ? (size_t)numBytesSent : 0;
}

size_t TCPSocket::Receive( void *buffer, size_t const maxByteSize )
{
	size_t receivedBytes = ::recv( m_handle, (char *)buffer, (int)maxByteSize - 1, 0 );

	return receivedBytes;
}

bool TCPSocket::IsClosed() const
{
	if( m_handle == INVALID_SOCKET )
		return true;
	else
		return false;
}

