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