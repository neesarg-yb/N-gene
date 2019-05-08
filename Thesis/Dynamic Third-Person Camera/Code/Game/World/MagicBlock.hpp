#pragma once
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/RaycastResult.hpp"

class Terrain;

class MagicBlock : public GameObject
{
public:
	 MagicBlock( Vector2 const &positionXZ, float offsetFromTerrain, Vector3 const &dimension, Vector3 const &movementDirection, float maxDistance, float time, Terrain const &parentTerrain, Clock *parentClock = nullptr );
	~MagicBlock();

private:
	Vector3		  m_spawnPosition		= Vector3::ZERO;
	Vector3	const m_size				= Vector3::ONE_ALL;
	Vector3	const m_movementDirection	= Vector3::RIGHT;
	float	const m_maxDistance			= 5.f;
	float	const m_time				= 2.f;

	Stopwatch	  m_timer;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	RaycastResult	DoPerfectRaycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const;
	Vector3			CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const;

private:
	bool	IsNearZero( float value ) const;
	float	GetSignedFloatMax( float value ) const;
};

inline bool MagicBlock::IsNearZero( float value ) const
{
	return fabsf( value ) < 0.0000001f;
}

inline float MagicBlock::GetSignedFloatMax( float value ) const
{
	return value < 0.f ? -FLT_MAX : FLT_MAX;
}

