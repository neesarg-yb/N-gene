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
	// If the socket is already being in use, close it
	if( m_handle != INVALID_SOCKET )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;
	}

	// Get our host address to bind with socket
	uint bindableAddressesCount = NetworkAddress::GetAllBindableAddresses( &m_address,		// Array to fill
																			1U,				// max number to return
																			port );			// desired port
	if( bindableAddressesCount == 0 )
	{
		ConsolePrintf( "Couldn't get a bindable address to host on!" );
		return false;
	}

	// Now we can try to bind the new host address
	// To do that, we need to create a new socket
	m_handle = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	sockaddr_storage	hostSocketAddress;
	size_t				hostSocketAddressLength;
	bool socketAddressIsValid = m_address.ToSocketAddress( (sockaddr*)&hostSocketAddress, &hostSocketAddressLength );

	if( !socketAddressIsValid )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;

		ConsolePrintf( "Couldn't get sockaddr from local host address!" );
		return false;
	}

	// Tell the OS that this address forwards to this socket
	int result = ::bind( m_handle, (sockaddr*)&hostSocketAddress, (int)hostSocketAddressLength );
	if( result == SOCKET_ERROR )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;

		ConsolePrintf( "Couldn't bind the new socket to host!" );
		return false;
	}

	// maxQueued: I can have these many people who can accept the connection
	result = ::listen( m_handle, maxQueued );
	if( result == SOCKET_ERROR )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;

		ConsolePrintf( "Can't start listening on hostSocket!" );
		return false;
	}

	// Once you're connected, you can accept the connection..
	ConsolePrintf( "Listening enabled.." );
	return true;
}

TCPSocket* TCPSocket::Accept()
{
	if( m_handle == INVALID_SOCKET )
		return nullptr;

	// Get their socket
	sockaddr_storage	theirSocketAddress;
	int					theirSocketAddressLength = sizeof(sockaddr_storage);
	SOCKET				theirSocket = ::accept( m_handle, (sockaddr*)&theirSocketAddress, &theirSocketAddressLength );

	// If not valid or not IPv4
	if( theirSocket == INVALID_SOCKET || theirSocketAddress.ss_family != AF_INET )
	{
		::closesocket( theirSocket );
		return nullptr;
	}

	// If valid
	TCPSocket *theirTCPSocket	= new TCPSocket();
	theirTCPSocket->m_handle	= theirSocket;
	theirTCPSocket->m_address	= NetworkAddress( (sockaddr*)&theirSocketAddress );

	return theirTCPSocket;
}

bool TCPSocket::Connect( NetworkAddress const &networkAddress )
{
	m_address	= networkAddress;
	m_handle	= ::socket( AF_INET,			// Address Family: IPv4
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
	bool				success = m_address.ToSocketAddress( (sockaddr*)&socketAddress, &addressLength );

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

bool TCPSocket::EnableNonBlocking()
{
	// non_blocking = 1, if not blocking
	u_long nonBlocking = 1;
	int zeroOnSuccess = ::ioctlsocket( (SOCKET)m_handle, FIONBIO, &nonBlocking );

	return (zeroOnSuccess == 0);
}

bool TCPSocket::DisableNonBlocking()
{
	// non_blocking = 0, if blocking
	u_long nonBlocking = 0;
	int zeroOnSuccess = ::ioctlsocket( (SOCKET)m_handle, FIONBIO, &nonBlocking );

	return (zeroOnSuccess == 0);
}

int TCPSocket::Send( void const *data, size_t const dataByteSize )
{
	int		numBytesSent = ::send( m_handle, (const char*)data, (int)dataByteSize, 0 );

	return numBytesSent;
}

int TCPSocket::Receive( void *buffer, size_t const maxByteSize )
{
	int receivedBytes = ::recv( m_handle, (char *)buffer, (int)maxByteSize, 0 );

	return receivedBytes;
}

bool TCPSocket::IsClosed() const
{
	if( m_handle == INVALID_SOCKET )
		return true;
	else
		return false;
}

