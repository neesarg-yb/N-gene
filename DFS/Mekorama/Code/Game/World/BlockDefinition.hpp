#pragma once
#include <map>
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Game/GameCommon.hpp"

class BlockDefinition;

typedef std::map< std::string, BlockDefinition* >	BlockDefinitionMap;

class BlockDefinition
{
private:
	 BlockDefinition() { }
	 BlockDefinition( const XMLElement& root );
	~BlockDefinition();

public:
	// Properties
	std::string	m_typeName				= "Name not assigned!";
	bool		m_isSolid				= true;

	// Material
	Material*	m_material				= nullptr;

	// UVs
	IntVector2	m_spriteSheetDimension	= IntVector2::ONE_ONE;
	IntVector2	m_uvTop					= IntVector2::ONE_ONE;
	IntVector2	m_uvSide				= IntVector2::ONE_ONE;
	IntVector2	m_uvBottom				= IntVector2::ONE_ONE;

public:
	// Static pool
	static BlockDefinitionMap s_definitions;

public:
	static void	LoadAllDefinitions( std::string pathToDefinitionXML );
	static void DeleteAllDefinition();
};