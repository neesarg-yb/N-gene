#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/SpriteAndAnimation/Sprite.hpp"

IntVector2 const ISO_DIR_AWAYLEFT		= IntVector2( -1,  1 );
IntVector2 const ISO_DIR_AWAYRIGHT		= IntVector2(  1,  1 );
IntVector2 const ISO_DIR_TOWARDLEFT		= IntVector2( -1, -1 );
IntVector2 const ISO_DIR_TOWARDRIGHT	= IntVector2(  1, -1 );

enum FacingDirection
{
	AWAY_LEFT = 0,
	AWAY_RIGHT,
	TOWARD_LEFT,
	TOWARD_RIGHT,
	NUM_FACING_DIRECTIONS
};

struct SpriteWithScale		// Members: Sprite*_sprite & Vec2_scale
{
	// Data Members
	Sprite*			srcSprite	= nullptr;
	Vector2			scale		= Vector2::ONE_ONE;

	// Constructor
	SpriteWithScale( char const * srcIdName, Vector2 const & scale )
	{
		this->srcSprite = Sprite::s_loadedSprites[ srcIdName ];
		this->scale		= scale;
	}

	SpriteWithScale()
	{
		this->srcSprite	= nullptr;
		this->scale		= Vector2::ONE_ONE;
	}
};

class IsoSprite
{
private:
	 IsoSprite( XMLElement const &isoSpriteElement );
	~IsoSprite();

private:
	std::string			m_idName = "";
	SpriteWithScale*	m_faces[ NUM_FACING_DIRECTIONS ];

public:
	static std::map< std::string, IsoSprite* > s_loadedIsoSprites;

	static void LoadAllIsoSpritesFromXML( char const * xmlFileName );
	static void DeleteAllIsoSprites();

	SpriteWithScale*	GetSpriteWithScaleForActorLookingInDirection( Vector3 const &actorForward );
};