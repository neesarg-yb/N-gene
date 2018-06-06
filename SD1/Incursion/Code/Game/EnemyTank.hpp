#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/EnemyBullet.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include<string>

class EnemyTank : public Entity
{
public:
	const std::string pathToSound = "Data//Audio//EnemyShoot.wav";
	SoundID tankFireSound;

	const float maxSpeed = 0.5f;			// Units per seconds
	const float bulletSpeed = 2.f;
	const float fireBulletsPerSeconds = 1.0f;
	Vector2 goalPosition = Vector2( 0.f , 0.f );
	const float goalPositionChangeAfterSeconds = 2.f;
	const std::string pathToImagePNG		= "Data//Images//EnemyTank4.png";
	Texture* tankTexture;
	AABB2 textureBounds;

	 EnemyTank();
	 EnemyTank( Vector2 position, float orientationInDegrees );
	~EnemyTank();

	void Render();
	void drawDebugInformations();
	void Update( float deltaSeconds );

	void moveAheadWithThrottle( float fractionUptoOne, float deltaTime );
	void rotateTankTowardsDegreeAngle( float degreeAngle, float deltaTime );

	EnemyBullet* createABullet( float deltaSeconds );
private:
	float timeElaspedSinceLastFire = 0.f;
	float timeElaspedSinceLastGoalChanged = 0.f;
};