#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Game/Potential Engine/GameObject.hpp"

class Terrain;

class House : public GameObject
{
public:
	 House( Vector2 positionXZ, float const height, float const width, float const length, float const wallThickness, Terrain const &parentTerrain );
	~House();

public:
	Vector3 const  m_size			= Vector3( 10.f, 10.f, 10.f );
	Terrain const &m_parentTerrain;

public:
	std::vector< AABB3 >	m_wallsWorldBounds;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const;
	Vector3			CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding );
};
