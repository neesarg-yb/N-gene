#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

class TowerDefinition;

typedef std::map< std::string, TowerDefinition* > TowerDefinitionMap;

class TowerDefinition
{
private:
	 TowerDefinition() { }
	 TowerDefinition( XMLElement const &root );
	~TowerDefinition();

public:
	// Properties
	std::string		m_towerName		= "Name not assigned";
	IntVector2		m_xzDimension	= IntVector2( 5, 5 );

	// Blocks
	std::vector< std::string >	m_blocksDefinitionList;

public:
	// Static pool
	static TowerDefinitionMap	s_definitions;

public:
	static void LoadDefinition( std::string pathToDefinitionXML );
	static void DeleteAllDefinitions();
};