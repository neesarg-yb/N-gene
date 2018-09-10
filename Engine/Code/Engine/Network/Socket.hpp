#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/Internal/WindowsCommon.hpp"

class Socket
{

public:
			 Socket() { }
	virtual ~Socket();

public:
	// When finished
	bool	Close();
	bool	IsClosed() const;

	// Traffic
	bool	EnableNonBlocking();
	bool	DisableNonBlocking();

public:
	SOCKET	m_handle = INVALID_SOCKET;

	// If you're listening, the address is YOUR address
	// If you're connecting (or socket is from an accept), the address is THEIR address
	//	THERE FOR =>
	//				Do not listen and connect on the same Socket!
	NetworkAddress	m_address;
};