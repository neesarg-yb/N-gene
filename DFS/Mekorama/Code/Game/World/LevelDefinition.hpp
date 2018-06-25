#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Game/GameCommon.hpp"

class LevelDefinition;

typedef std::map< std::string, LevelDefinition* > LevelDefinitionMap;

class LevelDefinition
{
private:
	 LevelDefinition() { }
	 LevelDefinition( XMLElement const &root );
	~LevelDefinition();

public:
	// Properties
	std::string		m_towerName			= "Name not assigned";
	IntVector3		m_spawnPlayerOn		= IntVector3::ZERO;
	IntVector3		m_spawnFinishAt		= IntVector3::ONE_ALL;

public:
	// Static pool
	static LevelDefinitionMap	s_definitions;

public:
	static void LoadDefinition( std::string pathToDefinitionXML );
	static void DeleteAllDefinitions();
};