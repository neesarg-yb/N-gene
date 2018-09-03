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
	 BlockDefinition( XMLElement const &root );
	~BlockDefinition();

public:
	// Properties
	std::string	m_typeName				= "Name not assigned!";
	bool		m_isSelectable			= true;
	bool		m_isDraggable			= false;
	bool		m_isSolid				= true;

	// Material
	Material*	m_material				= nullptr;

	// UVs
	IntVector2	m_spriteSheetDimension	= IntVector2::ONE_ONE;
	IntVector2	m_uvTopCoord			= IntVector2::ONE_ONE;
	IntVector2	m_uvSideCoord			= IntVector2::ONE_ONE;
	IntVector2	m_uvBottomCoord			= IntVector2::ONE_ONE;

public:
	// Static pool
	static BlockDefinitionMap s_definitions;

public:
	static void	LoadAllDefinitions( std::string pathToDefinitionXML );
	static void DeleteAllDefinitions();
};