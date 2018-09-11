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