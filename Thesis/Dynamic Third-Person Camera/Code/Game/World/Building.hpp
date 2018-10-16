#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Engine/Core/RaycastResult.hpp"

class Terrain;

class Building : public GameObject
{
public:
	 Building( Vector2 positionXZ, float const height, float const width, Terrain const &parentTerrain );
	~Building();

private:
	Vector3	const m_size		= Vector3( 10.f, 20.f, 10.f );
	Terrain	const &m_parentTerrain;

public:
	AABB3 m_worldBounds;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	bool			IsPointInside( Vector3 const &position ) const;
	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const;
	Vector3			CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const;
};
