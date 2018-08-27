#pragma once
#include "Engine/Input/Command.hpp"

class Network
{
public:
	static bool Startup();
	static void Shutdown();
};

// Console Commands
void NetworkMyIP		( Command &cmd );
void NetworkStartServer	( Command &cmd );
void NetworkStopServer	( Command &cmd );
void NetworkSendMessage	( Command &cmd );