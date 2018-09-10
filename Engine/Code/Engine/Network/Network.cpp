#pragma once
#include "Network.hpp"

#include <string>

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Internal/WindowsCommon.hpp"
#include "Engine/Network/NetworkAddress.hpp"
#include "Engine/Network/TCPSocket.hpp"

#pragma comment(lib, "ws2_32.lib" )	// WinSock libraries

bool serverIsRunning = false;

bool Network::Startup()
{
	// Pick the version we want; 
	WORD version = MAKEWORD( 2, 2 );

	// Initialize the (W)in(S)ock(A)PI.  
	WSADATA data;
	int error = ::WSAStartup( version, &data );

	// Console Commands
	CommandRegister( "networkMyIP", NetworkMyIP );

	GUARANTEE_RECOVERABLE( error == 0, "Error: Network starup, failed!" );
	return ( error == 0 );
}

void Network::Shutdown()
{
	::WSACleanup();
}

void NetworkMyIP( Command &cmd )
{
	UNUSED( cmd );

	NetworkAddress localAddresses[3];
	uint fetchedLocal = NetworkAddress::GetAllLocal( localAddresses, 3 );

	for( uint i = 0; i < fetchedLocal; i++ )
		ConsolePrintf( "(%u) My IP: %s", i+1U, localAddresses[i].IPToString().c_str() );
}