#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

struct sockaddr;

struct NetworkAddress
{
public:
	uint		addressIPv4;
	uint16_t	port;

public:
	NetworkAddress();
	NetworkAddress( sockaddr const *addr );
	NetworkAddress( char const *string );

public:
	bool ToSocketAddress( sockaddr *out, size_t *out_addrlen );
	bool FromSocketAddress( sockaddr const *sa );

	std::string ToString() const;

public:
	static NetworkAddress	GetLocal();
	static uint				GetAllLocal( NetworkAddress *out, uint maxCount );
	static uint				GetAllForHost( NetworkAddress *out, uint maxCount, char const *hostName, char const *serviceName );
};