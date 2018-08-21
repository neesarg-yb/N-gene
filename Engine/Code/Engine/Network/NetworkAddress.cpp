#pragma once
#include "NetworkAddress.hpp"
#include "Engine/Internal/WindowsCommon.hpp"

NetworkAddress::NetworkAddress()
{

}

NetworkAddress::NetworkAddress( sockaddr const *addr )
{
	UNUSED( addr );
}

NetworkAddress::NetworkAddress( char const *string )
{
	UNUSED( string );
}

bool NetworkAddress::ToSocketAddress( sockaddr *out, size_t *out_addrlen )
{
	UNUSED( out );
	UNUSED( out_addrlen );

	return false;
}

bool NetworkAddress::FromSocketAddress( sockaddr const *sa )
{
	UNUSED( sa );

	return false;
}

std::string NetworkAddress::ToString() const
{
	return std::string("INVALID");
}

NetworkAddress NetworkAddress::GetLocal()
{
	return NetworkAddress();
}

uint NetworkAddress::GetAllLocal( NetworkAddress *out, uint maxCount )
{
	UNUSED( out );
	UNUSED( maxCount );

	return 0U;
}

uint NetworkAddress::GetAllForHost( NetworkAddress *out, uint maxCount, char const *hostName, char const *serviceName )
{
	UNUSED( out );
	UNUSED( maxCount );
	UNUSED( hostName );
	UNUSED( serviceName );

	return 0U;
}
