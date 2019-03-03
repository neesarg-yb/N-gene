#pragma once
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"

class BlockDefinition
{
public:
	 BlockDefinition();
	 BlockDefinition( eBlockType type, bool isSolid, bool isFullyOpaque, bool isNeverVisible, int indoorLightLevel, IntVector2 const &sideTileCoord_TL, IntVector2 const &botTileCoord_TL, IntVector2 const &topTileCoord_TL );
	~BlockDefinition();

private:
	// Static Members
	static Material			*s_material;
	static SpriteSheet		*s_spriteSheet;
	static BlockDefinition	 s_definitions[ NUM_BLOCK_TYPES ];

public:
	// Definition Specific
	eBlockType	m_type				= BLOCK_AIR;

	bool		m_isSolid			= false;
	bool		m_isFullyOpaque		= false;
	bool		m_isNeverVisible	= true;
	int			m_indoorLightLevel	= 0;
	
	AABB2		m_uvSide			= AABB2::ONE_BY_ONE;
	AABB2		m_uvBottom			= AABB2::ONE_BY_ONE;
	AABB2		m_uvTop				= AABB2::ONE_BY_ONE;

public:
	static Material*				GetMaterial();
	static SpriteSheet*				GetSpriteSheet();
	static BlockDefinition const &	GetDefinitionForType( eBlockType type );

public:
	static void LoadDefinitions();
	static void DestroyDefinitions();
};

inline Material* BlockDefinition::GetMaterial()
{
	return s_material;
}

inline SpriteSheet* BlockDefinition::GetSpriteSheet()
{
	return s_spriteSheet;
}

inline BlockDefinition const & BlockDefinition::GetDefinitionForType( eBlockType type )
{
	return s_definitions[ type ];
}
