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
	 NetworkMessage() { }
	 NetworkMessage( const char *name );
	~NetworkMessage();

public:
//	When you hand it over (Final Buffer):
//	                                        Total Size <= ( 2 bytes + Network Packet Header Size + My Size )
//	|-----------------------------------------------------------------------
//	|  Paylooooooaaaaaad..
//	|-----------------------------------------------------------------------
//	

public:
	NetworkMessageHeader			 m_header;					// NetworkConnection fills the header when it adds to outgoing queue..
	NetworkMessageDefinition const	*m_definition	= nullptr;	// NetworkSession fills the header & definition when it receives the Packet

public:
	bool Write( int number );
	bool Write( float number );
	bool Write( std::string const &string );

	bool	Raad( int &outNumber ) const;
	bool	Read( float &outNumber ) const;
	size_t	Read( char *outString, size_t maxSize ) const;
};
