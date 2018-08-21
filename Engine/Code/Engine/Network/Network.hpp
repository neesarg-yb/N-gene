#include <string>

#include "Engine/Internal/WindowsCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/Command.hpp"

#pragma comment(lib, "ws2_32.lib" )	// WinSock libraries

void NetworkTestCmd( Command &cmd );

class Network
{
public:
	static bool Startup();
	static void Shutdown();
};

bool Network::Startup()
{
	// Pick the version we want; 
	WORD version = MAKEWORD( 2, 2 );

	// Initialize the (W)in(S)ock(A)PI.  
	WSADATA data;
	int error = ::WSAStartup( version, &data );

	// Console Command
	CommandRegister( "networkConnectTest", NetworkTestCmd );

	GUARANTEE_RECOVERABLE( error == 0, "Warning: Network starup, failed!" );
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

	hints.ai_family		= AF_INET;		// IPv4 address
	hints.ai_socktype	= SOCK_STREAM;	// TCP Socket ( SOCK_DGRAM for UDP )
	hints.ai_flags		= AI_PASSIVE;	// An address we can host on
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

// Net/NetAddress.hpp
bool GetAddressForHost( sockaddr *out, int *out_addrlen, char const * hostname, char const *service = "12345" )
{	
	/*
	if( StringIsNullOrEmpty( myName ) )
	{
		return;
	}
	*/

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


void NetworkTestCmd( Command &cmd )
{
	std::string addr_str = cmd.GetNextString();
	std::string ip;
	std::string port;

	char const *msg = cmd.GetNextString().c_str();
	/*
	if( StringIsNullOrEmpty(addr_str) || StringIsNullOrEmpty(msg) )
	{
		DebuggerPrintf( "\nMust provide an address and a message.\n" );
		return;
	}
	*/

//	addr_str.split( &ip, &port, ':' );
	std::vector<std::string> splitAddr = SplitIntoStringsByDelimiter( addr_str, ':' );
	ip = splitAddr[0];
	port = splitAddr[1];

	sockaddr_storage saddr;
	int addrlen;
	if( !GetAddressForHost( (sockaddr*)&saddr, &addrlen, ip.c_str(), port.c_str() ) )
	{
		DebuggerPrintf( "ERROR: NET, COULD NOT RESOLVE!!" );
		return;
	}

	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sock == INVALID_SOCKET )
	{
		DebuggerPrintf( "Could not create socket." );
	}

	int result = ::connect( sock, (sockaddr*)&saddr, (int)addrlen);
	if( result == SOCKET_ERROR )
	{
		DebuggerPrintf("ERROR, COULD NOT CONNECT!");
		return;
	}
	else
		int i = 0;
}