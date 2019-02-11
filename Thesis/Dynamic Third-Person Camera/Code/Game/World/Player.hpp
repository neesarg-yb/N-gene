#pragma once
#include "Engine/Core/GameObject.hpp"

class Terrain;

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain );
	~Player();

public:
	float	const	 m_bodyRadius		= 0.5f;
	Terrain const	*m_terrain			= nullptr;		// The terrain this player is on

	float	const	 m_mass				= 10.f;
	float	const	 m_friction			= 30.f;
	float	const	 m_xzMovementForce	= 1000.f;
	float			 m_defaultMaxSpeed	= 0.f;

	bool m_isPlayerOnTerrainSurface = false;

private:
	Vector3 m_cameraForward = Vector3::FRONT;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );
	void InformAboutCameraForward( Vector3 const &cameraForward );

	void ApplyResistantForces();
	void ApplyMovementForces();
	void CheckAndSnapOnTerrainSurface();

	void ApplyForce( float x, float y, float z );
	inline void ApplyForce( Vector3 force ) { ApplyForce( force.x, force.y, force.z ); }
};
