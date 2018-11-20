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
	NET_MESSAGE_OPTION_CONNECTIONLESS		= BIT_FLAG(0),
	NET_MESSAGE_OPTION_RELIABLE				= BIT_FLAG(1),
	NET_MESSAGE_OPTION_IN_ORDER				= BIT_FLAG(2),

	// Convenience
	NET_MESSAGE_OPTION_REQUIRES_CONNECTION	= 0U,
	NET_MESSAGE_OPTION_RELIABLE_IN_ORDER	= NET_MESSAGE_OPTION_RELIABLE | NET_MESSAGE_OPTION_IN_ORDER,
};

enum eNetworkCoreMessages : uint8_t
{
	NET_MESSAGE_PING = 0,
	NET_MESSAGE_PONG,
	NET_MESSAGE_HEARTBEAT,

	NET_MESSAGE_JOIN_REQUEST,				// unreliable
	NET_MESSAGE_JOIN_DENY,					// unreliable
	NET_MESSAGE_JOIN_ACCEPT,				// reliable in-order
	NET_MESSAGE_NEW_CONNECTION,				// reliable in-order
	NET_MESSAGE_JOIN_FINISHED,				// reliable in-order
	NET_MESSAGE_UPDATE_CONNECTION_STATE,	// reliable in-order

	NUM_CORE_NET_SESSION_MESSAGES
};

struct NetworkSender
{
public:
	NetworkSession		&session;
	NetworkAddress		 address;
	NetworkConnection	*connection;

public:
	NetworkSender( NetworkSession &parentSesion, NetworkAddress &netAddress, NetworkConnection *netConnection );
};

struct NetworkMessageHeader
{
	// Note!
	//		Don't forget to update "NETWORK_MESSAGE_HEADER_SIZE" in Engine/Core/EngineCommon.h
public:
	uint8_t		networkMessageDefinitionIndex	= 0xff;
	uint16_t	reliableID						= 0;
	uint16_t	sequenceID						= 0;
};

struct NetworkMessageDefinition
{
public:
	int						id			= -1;
	uint					channel		= 0U;
	std::string				name		= "NAME NOT ASSIGNED!";
	networkMessage_cb		callback	= nullptr;
	eNetworkMessageOptions	optionsFlag	= NET_MESSAGE_OPTION_REQUIRES_CONNECTION;

public:
	NetworkMessageDefinition() { }
	NetworkMessageDefinition( char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag );
	NetworkMessageDefinition( std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag );
	NetworkMessageDefinition( int id, char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag ); 
	NetworkMessageDefinition( int id, std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag ); 

public:
	bool RequiresConnection() const;
	bool IsReliable() const;
	bool IsInOrder() const;
};

class NetworkMessage : public BytePacker
{
public:
	 NetworkMessage( eEndianness endianness = LITTLE_ENDIAN );
	 NetworkMessage( const char *name, eEndianness endianness = LITTLE_ENDIAN );
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
	
	inline bool IsReliable() const	{ return m_definition->IsReliable(); }
	inline bool IsInOrder() const	{ return m_definition->IsInOrder(); }

	inline uint GetChannel() const	{ return m_definition->channel; }
};
