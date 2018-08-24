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
	NetworkAddress networkAddress;

	uint bindableAddressesCount = NetworkAddress::GetAllBindableAddresses( &networkAddress,		// Array to fill
																			1U,					// max number to return
																			12345 );			// desired port
	if( bindableAddressesCount == 0 )
	{
		ConsolePrintf( "Couldn't get a bindable address to host on!" );
		return false;
	}

	// Now we can try to bind the new host address
	// To do that, we need to create a new socket
	SOCKET hostSocket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	sockaddr_storage	hostSocketAddress;
	size_t				hostSocketAddressLength;
	bool socketAddressIsValid = networkAddress.ToSocketAddress( (sockaddr*)&hostSocketAddress, &hostSocketAddressLength );

	if( !socketAddressIsValid )
	{
		ConsolePrintf( "Couldn't get sockaddr from local host address!" );
		return false;
	}

	// Tell the OS that this address forwards to this socket
	int result = ::bind( hostSocket, (sockaddr*)&hostSocketAddress, hostSocketAddressLength );
	if( result == SOCKET_ERROR )
	{
		::closesocket( hostSocket );
		ConsolePrintf( "Couldn't bind the new socket to host!" );
		return false;
	}

	// maxQueued: I can have these many people who can accept the connection
	result = ::listen( hostSocket, maxQueued );
	if( result == SOCKET_ERROR )
	{
		::closesocket( hostSocket );
		ConsolePrintf( "Can't start listening on hostSocket!" );
		return false;
	}

	// Once you're connected, you can accept the connection..
	ConsolePrintf( "Listening enabled.." );
	return true;
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

