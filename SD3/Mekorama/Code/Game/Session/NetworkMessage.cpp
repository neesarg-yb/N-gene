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

bool NetworkMessage::Read( std::string &outString ) const
{
	UNUSED( outString );
	return false;
}

bool NetworkMessage::Read( float &outFloat ) const
{
	UNUSED( outFloat );
	return false;
}

