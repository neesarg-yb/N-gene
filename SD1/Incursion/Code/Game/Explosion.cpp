#include "Explosion.hpp"
#include "Game/GameCommon.hpp"

Explosion::Explosion( const Vector2& position, float radius, float duration )
{
	SetPosition(position);
	m_visualDisc2.radius = radius;

	explosionTexture = g_theRenderer->CreateOrGetTexture(pathToPNG);
	explosionSpriteSheet = new SpriteSheet(*explosionTexture , spriteSheetDimention.x , spriteSheetDimention.y);
	explosionAnimation = new SpriteAnimation(*explosionSpriteSheet, duration, SPRITE_ANIM_MODE_PLAY_TO_END, 0, 24 );
}

Explosion::~Explosion()
{
	delete explosionAnimation;
	delete explosionSpriteSheet;
}

void Explosion::Render()
{
	AABB2 boundsForAnimation = AABB2( GetPosition(), m_visualDisc2.radius, m_visualDisc2.radius);

	AABB2 textCoords = explosionAnimation->GetCurrentTexCoords();
	g_theRenderer->GLBlendChangeBeforeAnimation();
	g_theRenderer->DrawTexturedAABB( boundsForAnimation, explosionAnimation->GetTexture(), textCoords.mins, textCoords.maxs, RGBA_WHITE_COLOR);
	g_theRenderer->GLBlendRestoreAfterAnimation();
}

void Explosion::Update( float deltaSeconds )
{
	if( explosionAnimation->IsFinished() )
	{
		m_markForDelete = true;
		return;
	}

	explosionAnimation->Update(deltaSeconds);
}