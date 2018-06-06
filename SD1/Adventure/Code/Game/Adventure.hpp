#pragma once
#include <string>
#include <vector>
#include "Game/Player.hpp"
#include "Game/Map.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/GameCommon.hpp"

class Adventure
{
public:
	 Adventure( std::string adventureName, std::string adventureDefinitionName, Player& thePlayer );
	~Adventure();

	std::string						m_name;
	AdventureDefinition*			m_definition;

	Player&							m_thePlayer;
	std::map< std::string, Map* >	m_maps;
	Map*							m_currentMap;

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();

	void SetAsCurrentMap( std::string mapName );		// Changes m_currentMap & tells theCamera to change its bounds for the new map

private:

};