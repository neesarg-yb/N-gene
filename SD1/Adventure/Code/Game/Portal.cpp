#pragma once
#include "Portal.hpp"
#include "Game/Map.hpp"
#include "Game/theGame.hpp"

Portal::Portal( Vector2& position, std::string portalType, std::string destinationMap, std::string reciprocalPortalType )
	:Entity( position, 0.f, *PortalDefinition::s_definitions[ portalType ] )
{
	m_portalType			= portalType;
	m_destinationMapName	= destinationMap;
	m_reciprocalPortalType	= reciprocalPortalType;
}

Portal::~Portal()
{

}

void Portal::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);

	// If portal is disabled
	if( m_isDisabled )
	{
		Player& thePlayer	= g_theGame->m_currentAdventure->m_currentMap->m_currentPlayer;
		Disc2 playerDisc	= thePlayer.GetCollisionDisc();
		Disc2 poralDisc		= this->GetCollisionDisc();

		// Enable is after player leaves the portal
		if( Disc2::DoDiscsOverlap( playerDisc, poralDisc ) == false )
			m_isDisabled = false;
	}
}

void Portal::Render()
{
	Entity::Render();
}

Vector2 Portal::PrepPortalForPlayerSpawn()
{
	m_isDisabled = true;

	return m_position;
}