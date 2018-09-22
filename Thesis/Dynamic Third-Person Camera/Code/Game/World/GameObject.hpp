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
	Vector3 m_acceleration	= Vector3::ZERO;
	Vector3 m_velocity		= Vector3::ZERO;
	float	m_maxSpeed		= 10.f;

public:
	virtual void Update( float deltaSeconds );
	virtual void AddRenderablesToScene( Scene &activeScene )	= 0;
	virtual void RemoveRenderablesFromScene( Scene &activeScene ) = 0;

};