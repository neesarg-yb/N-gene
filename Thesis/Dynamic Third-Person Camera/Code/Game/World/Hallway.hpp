#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/RaycastResult.hpp"

class Terrain;

class Hallway : public GameObject
{
public:
	 Hallway( Vector2 positionXZ, float const height, float const width, float const entryLength, float const exitLength, float const wallThickness, Terrain const &parentTerrain );
	~Hallway();

public:
	float const m_height		= 05.f;
	float const m_width			= 04.f;
	float const m_entryLength	= 10.f;
	float const m_exitLength	= 05.f;

public:
	std::vector< AABB3 > m_wallsWorldBounds;
	Terrain const		&m_parentTerrain;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene ) override;
	void RemoveRenderablesFromScene( Scene &activeScene ) override;

	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const;
	Vector3			CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const;
};
