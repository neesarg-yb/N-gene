#pragma once
#include "Adventure.hpp"
#include "Game/Portal.hpp"
#include "Game/theGame.hpp"

Adventure::Adventure( std::string adventureName, std::string adventureDefinitionName, Player& thePlayer )
	: m_thePlayer( thePlayer )
{
	m_name			= adventureName;
	m_definition	= AdventureDefinition::s_definitions[ adventureDefinitionName ];

	// Create all the Maps
	for( std::map< std::string, MapData* >::iterator it = m_definition->m_maps.begin(); it != m_definition->m_maps.end(); it++ )
	{
		std::string newMapName = it->first;
		Map* newMap = new Map( it->first, it->second->mapDefinition->m_definitionName, thePlayer );
		m_maps[ newMapName ] = newMap;

		// Spawn all the actors in Map
		for( unsigned int i = 0; i < it->second->actors.size(); i++ )
		{
			ActorData* thisActor = it->second->actors[i];
			newMap->CreateActorOnTileType( thisActor->actorType, thisActor->onTileType, thisActor->spawnCount );
		}

		// Spawn all the portals in Map
		std::vector< PortalData* > allPortals = it->second->portals;
		for( unsigned int i = 0; i < allPortals.size(); i++ )
		{
			newMap->CreatePortal( allPortals[i]->portalType, allPortals[i]->onTileType, allPortals[i]->destinationMap, allPortals[i]->reciprocalPortalType );
		}
	}

	// For each map in the Adventure
	for( std::map< std::string, Map* >::iterator itMaps = m_maps.begin(); itMaps != m_maps.end(); itMaps++ )
	{
		// Find spawned portals
		Map* thisMap = itMaps->second;
		std::vector< Portal* > allPortalsThisMap = thisMap->GetAllPortals();
		for( Portal* currentPortal : allPortalsThisMap )
		{
			// If its reciprocal portal == nullptr
			if( currentPortal->m_reciprocalPortal == nullptr )
			{
				// Find its reciprocal map
				Map* reciprocalMap = m_maps[ currentPortal->m_destinationMapName ];
				// Find its reciprocal portal, whose reciprocal portal == nullptr, as well
				std::vector< Portal* > allPortalsInReciprocalMap = reciprocalMap->GetAllPortals();
				for( Portal* reciprocalPortal : allPortalsInReciprocalMap )
				{
					// Connect it
					if( reciprocalPortal->m_portalType == currentPortal->m_reciprocalPortalType && reciprocalPortal->m_reciprocalPortal == nullptr )
					{
						// Connect Portals
						currentPortal->m_reciprocalPortal		= reciprocalPortal;
						currentPortal->m_destinationMap			= reciprocalMap;

						reciprocalPortal->m_reciprocalPortal	= currentPortal;
						reciprocalPortal->m_destinationMap		= thisMap;
					}
				}
			}
		}
	}

	// Make startMap, the currentMap
	m_currentMap = m_maps[ m_definition->m_startMap ];
	m_currentMap->MoveActorToAWalkableTile();
}

Adventure::~Adventure()
{

}

void Adventure::BeginFrame()
{
	m_currentMap->BeginFrame();
}

void Adventure::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void Adventure::Render()
{
	m_currentMap->Render();
}

void Adventure::EndFrame()
{
	m_currentMap->EndFrame();
}

void Adventure::SetAsCurrentMap( std::string mapName )
{
	m_currentMap = m_maps[ mapName ];
	g_theGame->m_theCamera->PrepCameraForNewMap( *m_currentMap );
}