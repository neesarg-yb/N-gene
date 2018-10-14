#pragma once
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Game/GameCommon.hpp"

class Scene;

class GameObject
{
public:
			 GameObject();
			 GameObject( Vector3 position, Vector3 rotation, Vector3 scale );
	virtual ~GameObject();

public:
	// Raw Data
	Transform	 m_transform;
	Renderable	*m_renderable	= nullptr;

	// Physics Related
	Vector3 m_acceleration	= Vector3::ZERO;		// Gets reset, ever frame
	Vector3 m_velocity		= Vector3::ZERO;		// Does not gets reset, every frame
	float	m_maxSpeed		= 10.f;					// Velocity will get limited by max speed

public:
	virtual void Update( float deltaSeconds );
	virtual void AddRenderablesToScene( Scene &activeScene )	= 0;
	virtual void RemoveRenderablesFromScene( Scene &activeScene ) = 0;

public:
	virtual Vector3 CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const;		// returns the corrected position of center, if colliding

};