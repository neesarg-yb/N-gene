#pragma once
#include <map>
#include <string>
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/MapDefinition.hpp"

struct PortalData
{
	PortalData( std::string portalType, std::string onTileType, std::string destinationMap, std::string reciprocalPortalType );
										
	std::string portalType				= "No Data Set";
	std::string onTileType				= "No Data Set";
	std::string destinationMap			= "No Data Set";
	std::string reciprocalPortalType	= "No Data Set";
};

struct ActorData
{
	ActorData( std::string actorType, int spawnCount, std::string onTileType );

	int			spawnCount	= 1;
	std::string actorType	= "No Data Set";
	std::string onTileType	= "No Data Set";
};

struct MapData
{
	MapData( const XMLElement& mapElement );

	MapDefinition* mapDefinition;

	std::vector< PortalData* > portals;
	std::vector< ActorData* > actors;
};

class AdventureDefinition
{
public:
	static void LoadDefinitions( const XMLElement& rootElement );

	std::string m_adventureName		= "No Data Set";
	std::string m_adventureTitle	= "No Data Set";
	std::string m_startMap			= "No Data Set";
	std::string m_startOnTile		= "No Data Set";
	std::string m_winOnDeathOf		= "";
	std::string m_winOnCollectionOf	= "";
	std::map< std::string, MapData* > m_maps;


	static std::map< std::string, AdventureDefinition* > s_definitions;

private:
	 AdventureDefinition( const XMLElement& definitionElement );
	~AdventureDefinition();
};