#pragma once
#include "NetworkMessage.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

// STRUCT - NETWORK SENDER
NetworkSender::NetworkSender( NetworkSession &parentSesion, NetworkAddress &netAddress, NetworkConnection *netConnection )
	: session( parentSesion )
	, address( netAddress )
	, connection( netConnection ) 
{

}

// STRUCT - NETWORK MESSAGE DEFINITION
NetworkMessageDefinition::NetworkMessageDefinition( char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
{
	this->name			= name;
	this->callback		= callback;
	this->optionsFlag	= optionsFlag;
}

NetworkMessageDefinition::NetworkMessageDefinition( std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
{
	this->name			= name;
	this->callback		= callback;
	this->optionsFlag	= optionsFlag;
}

NetworkMessageDefinition::NetworkMessageDefinition( int id, char const *name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
{
	this->id			= id;
	this->name			= name;
	this->callback		= callback;
	this->optionsFlag	= optionsFlag;
}

NetworkMessageDefinition::NetworkMessageDefinition( int id, std::string &name, networkMessage_cb callback, eNetworkMessageOptions optionsFlag )
{
	this->id			= id;
	this->name			= name;
	this->callback		= callback;
	this->optionsFlag	= optionsFlag;
}

bool NetworkMessageDefinition::RequiresConnection() const
{
	return ( optionsFlag & NET_MESSAGE_OPTION_CONNECTIONLESS ) != NET_MESSAGE_OPTION_CONNECTIONLESS;
}

bool NetworkMessageDefinition::IsReliable() const
{
	return (optionsFlag & NET_MESSAGE_OPTION_RELIABLE) == NET_MESSAGE_OPTION_RELIABLE;
}


// CLASS - NETWORK MESSAGE
NetworkMessage::NetworkMessage( const char *name )
	: BytePacker( PACKET_MTU - NETWORK_PACKET_HEADER_SIZE - 2U - NETWORK_RELIABLE_MESSAGE_HEADER_SIZE , LITTLE_ENDIAN )
	, m_name( name )
{

}

NetworkMessage::~NetworkMessage()
{

}

bool NetworkMessage::Write( int number )
{
	return WriteBytes( sizeof(int), &number );
}

bool NetworkMessage::Write( float number )
{
	return WriteBytes( sizeof(float), &number );
}

bool NetworkMessage::Write( std::string const &string )
{
	return WriteString( string.c_str() );
}

bool NetworkMessage::Read( int &outNumber ) const
{
	size_t expectedBytes = sizeof( int );
	size_t bytesRead	 = ReadBytes( &outNumber, expectedBytes );

	return (expectedBytes == bytesRead);
}

bool NetworkMessage::Read( float &outNumber ) const
{
	size_t expectedBytes = sizeof( float );
	size_t bytesRead	 = ReadBytes( &outNumber, expectedBytes );

	return (expectedBytes == bytesRead);
}

size_t NetworkMessage::Read( char *outString, size_t maxSize ) const
{
	return ReadString( outString, maxSize );
}

NetworkMessageDefinition const* NetworkMessage::GetDefinition() const
{
	return m_definition;
}

void NetworkMessage::SetDefinition( NetworkMessageDefinition const *def )
{
	m_definition = def;

	m_name = m_definition->name;
	m_header.networkMessageDefinitionIndex = (uint8_t)def->id;
}

bool NetworkMessage::IsReliable() const
{
	return m_definition->IsReliable();
}
