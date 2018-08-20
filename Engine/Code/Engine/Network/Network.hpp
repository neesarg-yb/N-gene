// Add
// Engine/Code/Engine/Net/Net.hpp

// Order of following includes matters..
// That's why better have it under WindowsCommon.h file & replace all #include <Windows.h> with WindowsCommon.h
//
// Engine/Internal/WindowsCommon.h
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>	// upv6 (optional)
#include <Windows.h>

#include "Engine/Core/ErrorWarningAssert.hpp"

#pragma comment(lib, "ws2_32.lib" )	// winsock libraries

class Network
{
public:
	static bool Startup();
	static void Shutdown();
};

bool Network::Startup()
{
	WORD version = MAKEWORD( 2, 2 );

	WSADATA data;
	int error = ::WSAStartup( version, &data );

	GUARANTEE_OR_DIE( error == 0, "Networking Error!" );

	return ( error == 0 );
}

void Network::Shutdown()
{
	::WSACleanup();
}

// getting your address
void GetAddressExample()
{
	char myName[256];
	if( SOCKET_ERROR == ::gethostname( myName, 256 ) )
	{
		return;
	}

	char const *service = "80"; // service is like "http" or "ftp", which translates to a port (80 ot 21). We'll jusr use port 80 for this example
	
	/*
	if( StringIsNullOrEmpty( myName ) )
	{
		return;
	}
	*/

	addrinfo hints;
	memset( &hints, 0, sizeof(hints) ); // initalize to all zero

	hints.ai_family = AF_INET;			// IPv4 address
	hints.ai_socktype = SOCK_STREAM;	// TCP Socket ( SOCK_DGRAM for UDP )
	hints.ai_flags = AI_PASSIVE;		// An address we can host on
//	hints.ai_family |= AI_NUMERICHOST;	// Will speed up this function since it won't have to lookup the address;

	addrinfo *result = nullptr; 
	int status = getaddrinfo( myName, service, &hints, &result ); 
	if (status != 0) {
		DebuggerPrintf( "Failed to find addresses for [%s:%s].  Error[%s]", 
			myName, service, ::gai_strerror(status) );
		return; 
	}

	// result now is a linked list of addresses that match our filter; 
	addrinfo *iter = result;
	while (iter != nullptr) {
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 
			DebuggerPrintf( "My Address: %s", out ); 
		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

}