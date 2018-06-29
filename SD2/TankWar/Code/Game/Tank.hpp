#pragma once
#include "Game/GameObject.hpp"

class Turret;
class Terrain;
class Camera;

class Tank : public GameObject
{
public:
	 Tank( Vector2 const &spawnPosition, Terrain &isInTerrain, bool isPlayer, Camera* attachedCamera );
	~Tank();

public:
	Transform	m_anchorTransform;
	Transform	m_cameraSpringTransform;

	// If it is a Player Tank
	bool		m_isControlledByXbox		= false;
	Camera*		m_attachedCamera			= nullptr;

	// XZ Plane info
	Vector2		m_xzPosition				= Vector2::ZERO;
	Vector2		m_xzForward					= Vector2( 0.f, 1.f );
	Vector2		m_xzRight					= Vector2( 1.f, 0.f );

	float		m_height					= 1.f;
	float		m_speed						= 10.f;	// units per seconds
	float		m_rotationSpeed				= 35.f;	// degrees per seconds
	float		m_bulletsPerSecond			= 2.f;
	float		m_timeElapsedSinceLastFire	= 0.f;
	
	// Debug Trail
	float		m_spawnTrailPointAfter		= 0.2f;

	// Terrain
	Terrain&	m_parentTerrain;

	// Turret
	Turret*		m_turret					= nullptr;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

private:
	void ShootBullets( float deltaSeconds );
	void HandleInput( float deltaSeconds );		// Sets anchor transform's Y Rotation & m_xzPosition
	void DrawDebugAimCross();
};