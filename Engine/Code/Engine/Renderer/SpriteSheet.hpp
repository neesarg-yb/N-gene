#pragma once

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"

class SpriteSheet
{
public:
	const Texture& 	m_spriteSheetTexture; 	// Texture w/grid-based layout of sprites

	 SpriteSheet( const Texture& texture, int tilesWide, int tilesHigh );
	~SpriteSheet();
	
	AABB2 GetTexCoordsForSpriteCoords( IntVector2 spriteCoords ) const;			// Assumes that bottom-left coordinate is (0, 0)
	AABB2 GetTexCoordsForSpriteIndex( int spriteIndex ) const;					// Assumes that zero-index starts from top-left..
	int GetNumSprites() const;

private:
	IntVector2		m_spriteLayout;			// # of sprites across, and down, on the sheet
};