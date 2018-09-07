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

void ServerThread	( uint16_t port );
void ServiceThread	( TCPSocket *clientSocket );

bool Network::Startup()
{
	// Pick the version we want; 
	WORD version = MAKEWORD( 2, 2 );

	// Initialize the (W)in(S)ock(A)PI.  
	WSADATA data;
	int error = ::WSAStartup( version, &data );

	// Console Commands
	CommandRegister( "networkMyIP",			NetworkMyIP );
	CommandRegister( "networkStartServer",	NetworkStartServer );
	CommandRegister( "networkStopServer",	NetworkStopServer );
	CommandRegister( "networkSendMessage",	NetworkSendMessage );

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

void NetworkStartServer( Command &cmd )
{
	// Fetch port from command!
	std::string portStr = cmd.GetNextString();
	uint16_t	port	= 12345;
	if( portStr != "" )
		port = (uint16_t) atoi( portStr.c_str() );

	// If already running a server, shut it down to start a new one
	if( serverIsRunning == true )
	{
		ConsolePrintf( "Server is already running, stop it and try again.." );
		return;
	}

	std::thread serverThread( ServerThread, port );
	serverThread.detach();

	return;
}

void ServerThread( uint16_t port )
{
	// Start listening
	TCPSocket host;
	bool isListening = host.Listen( port, 16U );
	if( isListening == false )
	{
		serverIsRunning = false;
		ConsolePrintf( "Error: Server couldn't start listening!" );

		return;
	}

	// Everything is alright, spin up the thread
	serverIsRunning = true;
	while ( serverIsRunning )
	{
		TCPSocket *aClient = host.Accept();

		if( aClient != nullptr )
		{
			std::thread serviceThread( ServiceThread, aClient );
			serviceThread.detach();
		}
	}

	host.Close();
}

void ServiceThread( TCPSocket *clientSocket )
{
	size_t receivedSize = 0;
	while ( receivedSize == 0 )
	{
		char buffer[256];

		receivedSize = clientSocket->Receive( buffer, 256 );
		buffer[ receivedSize ] = '\n';

		std::string receivedStr( buffer );
		ConsolePrintf( "Received %u bytes: %s", receivedSize, receivedStr.c_str() );

		std::string sendStr = "We can talk now!";
		size_t sentBytes = clientSocket->Send( sendStr.c_str(), sendStr.size() );
		ConsolePrintf( "Sent %u bytes: %s", sentBytes, sendStr.c_str() );
	}

	// close & delete the client socket
	delete clientSocket;
	clientSocket = nullptr;
}

void NetworkStopServer( Command &cmd )
{
	UNUSED( cmd );

	TODO( "Learn how to stop Server Thread!" );
	serverIsRunning = false;
}

void NetworkSendMessage( Command &cmd )
{
	std::string hostAddressStr	= cmd.GetNextString();
	std::string msg				= cmd.GetNextString();
	if( hostAddressStr == "" )
		return;

	NetworkAddress	hostAddress = NetworkAddress( hostAddressStr.c_str() );
	TCPSocket		socket;		// defaults to blocking??

	if( socket.Connect( hostAddress ) )
	{
		ConsolePrintf( "Sending \"%s\" to %s ..", msg.c_str(), hostAddress.IPToString().c_str() );
		socket.Send( msg.c_str(), msg.size() );

		char payload[256];
		size_t recvd	 = socket.Receive( payload, 256 );
		payload[ recvd ] = NULL;
		ConsolePrintf( "Received: %s", payload );

		socket.Close();
	}
	else
		ConsolePrintf( "Error: Could not connect to %s!", hostAddress.IPToString().c_str() );
}