#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/Network/NetworkAddress.hpp"

class  NetworkMessage;
class  NetworkConnection;
class  NetworkSession;
struct NetworkSender;

typedef bool (*networkMessage_cb) ( NetworkMessage const &, NetworkSender & );

enum eNetworkMessageOptions : uint
{
	NET_MESSAGE_OPTION_CONNECTIONLESS					= BIT_FLAG(0),
	NET_MESSAGE_OPTION_RELIABLE							= BIT_FLAG(1),
	NET_MESSAGE_OPTION_IN_ORDER							= BIT_FLAG(2),

	// Convenience
	NET_MESSAGE_OPTION_REQUIRES_CONNECTION				= 0U,
	NET_MESSAGE_OPTION_RELIABLE_IN_ORDER				= NET_MESSAGE_OPTION_RELIABLE | NET_MESSAGE_OPTION_IN_ORDER,
};

enum eNetworkCoreMessages : uint8_t
{
	NET_MESSAGE_PING = 0,
	NET_MESSAGE_PONG,
	NET_MESSAGE_HEARTBEAT,

	NUM_NET_MESSAGES
};

struct NetworkSender
{
public:
	NetworkSession		&session;
	NetworkAddress		 address;
	NetworkConnection	*connection;

public:
	NetworkSender( NetworkSession &parentSesion, NetworkAddress &netAddress, NetworkConnection *netConnection )
		: session( parentSesion )
		, address( netAddress )
		, connection( netConnection ) { }
};

struct NetworkMessageHeader
{
	// Note!
	//		Don't forget to update "NETWORK_MESSAGE_HEADER_SIZE" in Engine/Core/EngineCommon.h
public:
	uint8_t		networkMessageDefinitionIndex	= 0xff;
	uint16_t	reliableID						= 0;
};

struct NetworkMessageDefinition
{
public:
	int						id			= -1;
	std::string				name		= "NAME NOT ASSIGNED!";
	networkMessage_cb		callback	= nullptr;
	eNetworkMessageOptions	optionsFlag	= NET_MESSAGE_OPTION_REQUIRES_CONNECTION;

public:
	NetworkMessageDefinition() { }
	NetworkMessageDefinition( char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
	{
		this->name			= name;
		this->callback		= callback;
		this->optionsFlag	= optionsFlag;
	}
	NetworkMessageDefinition( std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
	{
		this->name			= name;
		this->callback		= callback;
		this->optionsFlag	= optionsFlag;
	}
	NetworkMessageDefinition( int id, char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag ) 
	{
		this->id			= id;
		this->name			= name;
		this->callback		= callback;
		this->optionsFlag	= optionsFlag;
	}
	NetworkMessageDefinition( int id, std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag ) 
	{
		this->id			= id;
		this->name			= name;
		this->callback		= callback;
		this->optionsFlag	= optionsFlag;
	}

public:
	bool RequiresConnection() const
	{
		return ( optionsFlag & NET_MESSAGE_OPTION_CONNECTIONLESS ) != NET_MESSAGE_OPTION_CONNECTIONLESS;
	}

	bool IsReliable() const
	{
		return (optionsFlag & NET_MESSAGE_OPTION_RELIABLE) == NET_MESSAGE_OPTION_RELIABLE;
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
	uint64_t						 m_lastSentHPC	= 0U;
	NetworkMessageHeader			 m_header;					// NetworkConnection fills the header when it adds to outgoing queue..
private:
	NetworkMessageDefinition const	*m_definition	= nullptr;	// *NetworkSession fills the header & definition when it receives the Packet

public:
	bool Write( int number );
	bool Write( float number );
	bool Write( std::string const &string );

	bool	Read( int &outNumber ) const;
	bool	Read( float &outNumber ) const;
	size_t	Read( char *outString, size_t maxSize ) const;

public:
	NetworkMessageDefinition const* GetDefinition() const;
	void SetDefinition( NetworkMessageDefinition const *def );	// Sets message definition and updates the member variables: m_name, m_header
	
	bool IsReliable() const;
};
