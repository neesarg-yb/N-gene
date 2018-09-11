#pragma once
#include "Socket.hpp"

Socket::~Socket()
{
	if( m_handle != INVALID_SOCKET )
	{
		::closesocket( m_handle );
		m_handle = INVALID_SOCKET;
	}
}

bool Socket::Close()
{
	if( m_handle != INVALID_SOCKET )
	{
		int zeroOnSuccess = ::closesocket( m_handle );
		bool closed = (zeroOnSuccess == 0);

		if( closed )
			m_handle = INVALID_SOCKET;

		return closed;
	}
	else
		return true;
}

bool Socket::IsClosed() const
{
	return (m_handle == INVALID_SOCKET);
}

bool Socket::EnableNonBlocking()
{
	// non_blocking = 1, if not blocking
	u_long nonBlocking = 1;
	int zeroOnSuccess = ::ioctlsocket( (SOCKET)m_handle, FIONBIO, &nonBlocking );

	return (zeroOnSuccess == 0);
}

bool Socket::DisableNonBlocking()
{
	// non_blocking = 0, if blocking
	u_long nonBlocking = 0;
	int zeroOnSuccess = ::ioctlsocket( (SOCKET)m_handle, FIONBIO, &nonBlocking );

	return (zeroOnSuccess == 0);
}

