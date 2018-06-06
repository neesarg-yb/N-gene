#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/PlayerBullet.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include<string>

class PlayerTank : public Entity
{
public:
	bool isInvisible = false;
	float gunOverheatFraction = 0.f;
	float currentOverheatInTimeUnits = 0.f;
	const float maxOverheatAfterTime = 5.0f;

	const std::string pathToSound = "Data//Audio//PlayerShootNormal.ogg";
	SoundID tankFireSound;

	const float maxSpeed = 1.f;			// Units per seconds
	const float bulletSpeed = 2.f;
	float fireBulletsPerSeconds = 5.f;
	const std::string pathToBasePNG		= "Data//Images//PlayerTankBase.png";
	const std::string pathToTurretPNG	= "Data//Images//PlayerTankTop.png";
	Texture* tankTexture;
	Texture* turretTexture;
	float turretOrientationInDegrees;
	float turretRotationSpeedDegreesPerSeconds;
	AABB2 textureBounds;

	 PlayerTank();
	 PlayerTank( Vector2 position, float orientationInDegrees );
	~PlayerTank();
	
	void Render();
	void Update( float deltaSeconds );

	void moveAheadWithThrottle( float fractionUptoOne, float deltaTime );
	void rotateTankTowardsDegreeAngle( float degreeAngle, float deltaTime );
	void rotateTurretTowardsDegreeAngle( float degreeAngle, float deltaTime );

	PlayerBullet* createABullet( float deltaSeconds );
	void TakeOneHitFromBullet();
private:
	float timeElaspedSinceLastFire = 0.f;
	float defaultFireRate;
};