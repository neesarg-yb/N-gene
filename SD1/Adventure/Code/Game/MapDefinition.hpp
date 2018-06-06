#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/../ThirdParty/tinyxml/tinyxml2.h"
#include "Game/MapGenStep.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;

class MapDefinition
{
public:
	~MapDefinition();

	std::string					m_definitionName = "No definition name assigned";

	int							m_width = 0;
	int							m_height = 0;
	std::string					m_defaultTile = "No default Tile set";
	std::vector< MapGenStep* >	m_mapGenSteps;

	static std::map< std::string, MapDefinition* >	s_definitions;
	static void LoadMapDefinitions( const XMLElement& root );

private:
	MapDefinition();
	MapDefinition( const XMLElement& );
};