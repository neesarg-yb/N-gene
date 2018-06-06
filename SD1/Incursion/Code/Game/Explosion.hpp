#pragma once

#include "Entity.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"


class Explosion : public Entity
{
public:
	 Explosion( const Vector2& position, float radius, float duration );
	~Explosion();

	std::string pathToPNG = "Data//Images//Explosion_5x5.png";
	Texture* explosionTexture;
	IntVector2 spriteSheetDimention = IntVector2( 5 , 5);
	SpriteSheet* explosionSpriteSheet;
	SpriteAnimation* explosionAnimation;

	void Render();
	void Update( float deltaSeconds );
private:

};