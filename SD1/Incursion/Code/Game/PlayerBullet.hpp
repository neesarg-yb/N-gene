#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>

class PlayerBullet : public Entity
{
public:
	const std::string pathToBulletPNG = "Data//Images//Bullet.png";
	Texture*	  bulletTexture;
	AABB2 textureBounds;

	 PlayerBullet( Vector2 position, float shootAtAngle, float speed );
	~PlayerBullet();

	void Render();
	void Update( float deltaSeconds );
private:

};