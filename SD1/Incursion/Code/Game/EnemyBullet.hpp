#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>

class EnemyBullet : public Entity
{
public:
	const std::string pathToBulletPNG = "Data//Images//Bullet.png";
	Texture*	 bulletTexture;
	AABB2		 textureBounds;

	 EnemyBullet( Vector2 position, float shootAtAngle, float speed );
	~EnemyBullet();

	void Render();
	void Update( float deltaSeconds );
private:

};