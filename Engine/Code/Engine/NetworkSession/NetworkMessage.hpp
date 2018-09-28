#pragma once
#include <string>
#include "Engine/Network/BytePacker.hpp"

class NetworkMessage;
class NetworkConnection;

typedef bool (*networkMessage_cb) ( NetworkMessage const &, NetworkConnection & );

struct NetworkMessageHeader
{
	uint8_t networkMessageDefinitionIndex = 0xff;
};

struct NetworkMessageDefinition
{
public:
	int					id			= -1;
	std::string			name		= "NAME NOT ASSIGNED!";
	networkMessage_cb	callback	= nullptr;

public:
	NetworkMessageDefinition() { }
	NetworkMessageDefinition( char const *name, networkMessage_cb callback )
	{
		this->name		= name;
		this->callback	= callback;
	}
	NetworkMessageDefinition( std::string &name, networkMessage_cb callback )
	{
		this->name		= name;
		this->callback	= callback;
	}
	NetworkMessageDefinition( int id, std::string &name, networkMessage_cb callback ) 
	{
		this->id		= id;
		this->name		= name;
		this->callback	= callback;
	}
};

class NetworkMessage : public BytePacker
{
public:
	 NetworkMessage( std::string &name );
	~NetworkMessage();

public:
//	When you hand it over (Final Buffer):
//	                                        Total Size <= ( 2 bytes + Network Packet Header Size + My Size )
//	|-----------------------------------------------------------------------
//	|  Paylooooooaaaaaad..
//	|-----------------------------------------------------------------------
//	

public:
	std::string						 m_name;
	NetworkMessageHeader			 m_header;
	NetworkMessageDefinition const	*m_definition	= nullptr;
};
