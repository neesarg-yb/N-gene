#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

struct sockaddr;

struct NetworkAddress
{
public:
	uint		addressIPv4			= 0U;	// 192.168.1.20 => 192168120
	uint16_t	port				= 0;	// 12345 => 12345

public:
	NetworkAddress() { }
	NetworkAddress( sockaddr const *addr );
	NetworkAddress( char const *addrString );

public:
	bool ToSocketAddress( sockaddr *out, size_t *out_addrlen ) const;
	void FromSocketAddress( sockaddr const *sa );

	std::string ToString() const;

public:
	static NetworkAddress	GetLocal();
	static uint				GetAllLocal( NetworkAddress *out, uint maxCount );
	static uint				GetAllForHost( NetworkAddress *out, uint maxCount, char const *hostName, char const *serviceName = "12345" );
	static bool				GetSocketAddressForHost( sockaddr *out, int *out_addrlen, char const * hostname, char const *service = "12345" );
};