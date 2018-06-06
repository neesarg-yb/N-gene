#include "SpriteSheet.hpp"


SpriteSheet::SpriteSheet( const Texture& texture, int tilesWide, int tilesHigh ) 
	: m_spriteSheetTexture(texture)
{
	m_spriteLayout = IntVector2( tilesWide , tilesHigh );
}

SpriteSheet::~SpriteSheet()
{

}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords( IntVector2 spriteCoords ) const 
{
	// If passed some crazy spriteCoords, short-circuit it
	if( spriteCoords.x >= m_spriteLayout.x || spriteCoords.y >= m_spriteLayout.y ) {
		IntVector2 lastCoord = IntVector2( m_spriteLayout.x - 1 , m_spriteLayout.y - 1 );
		return GetTexCoordsForSpriteCoords( lastCoord );
	}

	// Otherwise, normal spriteCoords: Handle it appropriately 
	Vector2 min = Vector2( (spriteCoords.x + 1) * ( 1.f / m_spriteLayout.x ) , spriteCoords.y * ( 1.f / m_spriteLayout.y ) );				// ( ( (spCoo.x+1)   * 1/w) ,  (  spCoo.y   * 1/h)  )
	Vector2 max = Vector2( spriteCoords.x * ( 1.f / m_spriteLayout.x )       , (spriteCoords.y + 1.f) * ( 1.f / m_spriteLayout.y ) );		// ( ( spCoo.x * 1/w) , ( (spCoo.y+1) * 1/h) )


	/* POSSIBLE FUTURE UNEXPECTED BEHAVIOURS BECAUSE OF THE CHANGES AFTER THIS COMMENT

	// Old code was,
		AABB2 textCoords = AABB2( min , max );

		return textCoords;

	// But it produced upside-down & horizontally-flipped images..
	//	So I flipped them again, to make it correct

	*/
	Vector2 old_min = min;
	min.y = max.y;
	max.y = old_min.y;

	AABB2 textCoords = AABB2( max , min );
	
	return textCoords;
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex( int spriteIndex ) const 
{
	IntVector2 spriteCoords;
	spriteCoords.y = (int) ( spriteIndex / m_spriteLayout.x );					// y = (index / width)
	spriteCoords.x = spriteIndex - ( spriteCoords.y * m_spriteLayout.x );		// x = index - ( y * width )

	// POSSIBLE FUTURE UNEXPECTED BEHAVIOURS BECAUSE OF " spriteCoords.y = m_spriteLayout.y - spriteCoords.y - 1; " line
	// OpenGL new context: Previously ( 0, 0 ) started at top-left; now it is at bottom-left. So flip the y-coord to work with same logic..
	spriteCoords.y = m_spriteLayout.y - spriteCoords.y - 1;

	return GetTexCoordsForSpriteCoords(spriteCoords);
}

int SpriteSheet::GetNumSprites() const {
	return m_spriteLayout.x * m_spriteLayout.y;
}