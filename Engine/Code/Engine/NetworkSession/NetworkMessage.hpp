#pragma once
#include <string>
#include "Engine/Network/BytePacker.hpp"
#include "Engine/Network/NetworkAddress.hpp"

class  NetworkMessage;
class  NetworkConnection;
class  NetworkSession;
struct NetworkSender;

typedef bool (*networkMessage_cb) ( NetworkMessage const &, NetworkSender & );

struct NetworkSender
{
public:
	NetworkSession		&session;
	NetworkAddress		 address;
	NetworkConnection	*connection;

public:
	NetworkSender( NetworkSession &parentSesion, NetworkAddress &netAddress, NetworkConnection *netConnection )
		: session( parentSesion )
	{
		address		= netAddress;
		connection	= netConnection;
	}
};

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
	std::string						 m_name			= "NAME NOT ASSIGNED!";
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
