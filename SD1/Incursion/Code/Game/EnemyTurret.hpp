#pragma once

#include "Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/EnemyBullet.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class EnemyTurret : public Entity
{
public:

	const std::string pathToSound = "Data//Audio//EnemyShoot.wav";
	SoundID turretFireSound;

	const float bulletSpeed = 2.f;
	const float fireBulletsPerSeconds = 1.f;

	const std::string pathToBasePNG		= "Data//Images//EnemyTurretBase.png";
	const std::string pathToTurretPNG	= "Data//Images//EnemyTurretTop.png";
	Texture* baseTexture;
	Texture* turretTexture;
	float turretOrientationInDegrees;
	float turretRotationSpeedDegreesPerSeconds;
	AABB2 textureBounds;

	 EnemyTurret( const Vector2& position, float orientationDegrees );
	~EnemyTurret();

	void Render();
	void Update( float deltaSeconds );
	EnemyBullet* createABullet( float deltaSeconds );
private:

	float timeElaspedSinceLastFire = 0.f;
};