#pragma once
#include <vector>
#include "NetworkAddress.hpp"
#include "Engine/Internal/WindowsCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/StringUtils.hpp"

NetworkAddress::NetworkAddress( sockaddr const *addr )
{
	FromSocketAddress( addr );
}

NetworkAddress::NetworkAddress( char const *addrString )
{
	std::string ipAddress;
	std::string portStr = "12345";

	// Split IP and Port from addrString
	std::vector< std::string > splitStrings = SplitIntoStringsByDelimiter( std::string(addrString), ':' );
	
	// Set IP and Port strings
	if( splitStrings.size() >= 2 )
	{
		ipAddress	= splitStrings[0];
		portStr		= splitStrings[1];
	}
	else
		ipAddress	= splitStrings[0];

	// Get Address for that host
	sockaddr	socketAddressOut;
	int			socketAddressLength;
	bool success = GetSocketAddressForHost( &socketAddressOut, &socketAddressLength, ipAddress.c_str(), portStr.c_str() );

	GUARANTEE_RECOVERABLE( success, "NetworkAddress: Failed to get socket address!" );
	FromSocketAddress( &socketAddressOut );
}

bool NetworkAddress::ToSocketAddress( sockaddr *out, size_t *out_addrlen )
{
	if( addressIPv4 == 0U && port == 0 )
		return false;

	sockaddr_in out_in;
	out_in.sin_addr.S_un.S_addr	= addressIPv4;
	out_in.sin_port				= port;
	out_in.sin_family			= AF_INET;

	memcpy( out, &out_in, sizeof(sockaddr_in) );
	*out_addrlen = sizeof(sockaddr_in);
	
	return true;
}

void NetworkAddress::FromSocketAddress( sockaddr const *sa )
{
	sockaddr_in *inAddr = (sockaddr_in*) sa;

	addressIPv4	= inAddr->sin_addr.S_un.S_addr;
	port		= inAddr->sin_port;
}

std::string NetworkAddress::ToString() const
{
	char outAddress[256];
	inet_ntop( AF_INET, &addressIPv4, outAddress, 256 );

	return std::string( outAddress );
}

NetworkAddress NetworkAddress::GetLocal()
{
	NetworkAddress toReturn;
	uint numLocal = GetAllLocal( &toReturn, 1U );
	
	GUARANTEE_RECOVERABLE( numLocal >= 1U, "Can't get any local addresses!!" );
	return toReturn;
}

uint NetworkAddress::GetAllLocal( NetworkAddress *out, uint maxCount )
{
	// Get local host's name and service
	char		 myName[256];
	char const	*service = "80";		// service is like "http" or "ftp", which translates to a port (80 or 21). We'll just use port 80 for this example
	if( SOCKET_ERROR == ::gethostname( myName, 256 ) )
		return 0U;
	if( StringIsNullOrEmpty( myName ) )
		return 0U;

	// Setup hints
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) ); // initialize to all zero
	hints.ai_family		= AF_INET;		// IPv4 address
	hints.ai_socktype	= SOCK_STREAM;	// TCP Socket ( SOCK_DGRAM for UDP )
	hints.ai_flags		= AI_PASSIVE;	// An address we can host on

	// Get result
	addrinfo	*result = nullptr; 
	int			 status = getaddrinfo( myName, service, &hints, &result ); 
	if (status != 0) 
	{
		GUARANTEE_RECOVERABLE( false, "\nFailed to find addresses for [%s:%s].  Error[%s]\n", myName, service, ::gai_strerror(status) );
		return 0U;
	}

	// Result is a linked list of addresses that match our filter
	addrinfo	*iter		= result;
	uint		 numResults	= 0U;
	while ( iter != nullptr ) 
	{
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if ( iter->ai_family == AF_INET  && numResults < maxCount ) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			out[ numResults ] = NetworkAddress( (sockaddr*)ipv4 );

			numResults++;
		}

		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return numResults;
}

uint NetworkAddress::GetAllForHost( NetworkAddress *out, uint maxCount, char const *hostName, char const *serviceName /* = "12345" */ )
{
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) ); // initialize to all zero
	hints.ai_family		= AF_INET;		// IPv4 address
	hints.ai_socktype	= SOCK_STREAM;	// TCP Socket ( SOCK_DGRAM for UDP )

	addrinfo *result = nullptr; 
	int status = getaddrinfo( hostName, serviceName, &hints, &result ); 
	if (status != 0)
		return 0U;

	addrinfo	*iter		= result;
	uint		 numResults	= 0U;
	while (iter != nullptr) 
	{
		if ( iter->ai_family == AF_INET && numResults < maxCount ) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			out[ numResults ] = NetworkAddress( (sockaddr*)ipv4 );

			numResults++;
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return numResults;
}

bool NetworkAddress::GetSocketAddressForHost( sockaddr *out, int *out_addrlen, char const * hostname, char const *service /* = "12345" */ )
{
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) ); // initialize to all zero

	hints.ai_family = AF_INET;			// IPv4 address
	hints.ai_socktype = SOCK_STREAM;	// TCP Socket ( SOCK_DGRAM for UDP )
										//	hints.ai_flags = AI_PASSIVE;		// WE DON'T NEED IT HERE, B/C WE'RE CONNECTING TO A HOST
										//	hints.ai_family |= AI_NUMERICHOST;	// Will speed up this function since it won't have to lookup the address;

	addrinfo *result = nullptr; 
	int status = getaddrinfo( hostname, service, &hints, &result ); 
	if (status != 0) {
		return false; 
	}

	addrinfo *iter = result;
	bool found_one = false;
	while (iter != nullptr) {

		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 

			memcpy( out, ipv4, sizeof(sockaddr_in) );
			*out_addrlen = sizeof( sockaddr_in );
			found_one = true;
			break;
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return found_one;
}