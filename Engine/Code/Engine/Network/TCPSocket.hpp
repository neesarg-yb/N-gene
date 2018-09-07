#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/Internal/WindowsCommon.hpp"

class TCPSocket
{
public:
	 TCPSocket() { }
	~TCPSocket();

public:
	// Starting and Stopping
	bool		Listen( uint16_t port, uint maxQueued );
	TCPSocket*	Accept();											// Returns client's socket, if received anything..

	// For joining
	bool	Connect( NetworkAddress const &networkAddress );

	// When finished
	void	Close();

	// Traffic
	bool	EnableNonBlocking();
	bool	DisableNonBlocking();
	size_t	Send( void const *data, size_t const dataByteSize );	// returns "how much got sent?"
	size_t	Receive( void *buffer, size_t const maxByteSize );		// returns "how much got received?"

	// Helpers
	bool	IsClosed() const;

public:
	SOCKET	m_handle = INVALID_SOCKET;

	// If you're listening, the address is YOUR address
	// If you're connecting (or socket is from an accept), the address is THEIR address
	//	THERE FOR =>
	//				Do not listen and connect on the same Socket!
	NetworkAddress	m_address;
};