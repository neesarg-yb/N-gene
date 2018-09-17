#pragma once
#include "NetworkMessage.hpp"
#include "Game/GameCommon.hpp"

NetworkMessage::NetworkMessage( char const *msgName )
{
	UNUSED( msgName );
}

NetworkMessage::~NetworkMessage()
{

}

bool NetworkMessage::Read( float &outFloat ) const
{
	UNUSED( outFloat );
	return false;
}

bool NetworkMessage::Read( std::string &outString ) const
{
	UNUSED( outString );
	return false;
}

void NetworkMessage::Write( float number )
{
	UNUSED( number );
}

void NetworkMessage::Write( std::string &message )
{
	UNUSED( message );
}

