#pragma once
#include "MagicBlock.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/World/Terrain.hpp"

MagicBlock::MagicBlock( Vector2 const &positionXZ, float offsetFromTerrain, Vector3 const &dimension, Vector3 const &movementDirection, float maxDistance, float time, Terrain const &parentTerrain, Clock *parentClock /* = nullptr */ )
	: m_timer( parentClock )
	, m_size( dimension )
	, m_movementDirection( movementDirection )
	, m_maxDistance( maxDistance )
	, m_time( time )
{
	// Set Transform
	m_spawnPosition = parentTerrain.Get3DCoordinateForMyPositionAt( positionXZ, (m_size.y * 0.5f) + offsetFromTerrain );
	m_transform = Transform( m_spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Set Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *mesh = MeshBuilder::CreateCube( m_size, Vector3::ZERO, RGBA_ORANGE_COLOR );
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material *material = Material::CreateNewFromFile( "Data\\Materials\\building.material" );
	m_renderable->SetBaseMaterial( material );

	m_timer.SetTimer( m_time );
}

MagicBlock::~MagicBlock()
{
	delete m_renderable;
	m_renderable = nullptr;
}

void MagicBlock::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_timer.CheckAndReset();
	float t = m_timer.GetNormalizedElapsedTime() * 360.f;

	Vector3 newPosition = m_spawnPosition + ( m_movementDirection * SinDegree( t ) * m_maxDistance );
	m_transform.SetPosition( newPosition );
}

void MagicBlock::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void MagicBlock::RemoveRenderablesFromScene( Scene &activeScene )
{
	activeScene.RemoveRenderable( *m_renderable );
}

RaycastResult MagicBlock::DoPerfectRaycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const
{
	PROFILE_SCOPE_FUNCTION();

	Vector3	const rayDispacement = direction * maxDistance;
	Vector3	const rayEndPosition = startPosition + rayDispacement;
	RaycastResult hitResult = RaycastResult( rayEndPosition );			// Set up as if "NO IMPACT"

	// How much it costs to move on the axis, in terms of parametric "t"
	float const xDeltaT = IsNearZero( rayDispacement.x ) ? GetSignedFloatMax( rayDispacement.x ) : (1.f / rayDispacement.x);
	float const yDeltaT = IsNearZero( rayDispacement.y ) ? GetSignedFloatMax( rayDispacement.y ) : (1.f / rayDispacement.y);
	float const zDeltaT = IsNearZero( rayDispacement.z ) ? GetSignedFloatMax( rayDispacement.z ) : (1.f / rayDispacement.z);

	// What is range of "t" when the raycast is inside the building?
	AABB3 worldBounds = AABB3( m_transform.GetWorldPosition(), m_size.x, m_size.y, m_size.z );
	FloatRange xTRange;
	xTRange.ExpandToInclude( (worldBounds.mins.x - startPosition.x) * xDeltaT );
	xTRange.ExpandToInclude( (worldBounds.maxs.x - startPosition.x) * xDeltaT );

	FloatRange yTRange;
	yTRange.ExpandToInclude( (worldBounds.mins.y - startPosition.y) * yDeltaT );
	yTRange.ExpandToInclude( (worldBounds.maxs.y - startPosition.y) * yDeltaT );

	FloatRange zTRange;
	zTRange.ExpandToInclude( (worldBounds.mins.z - startPosition.z) * zDeltaT );
	zTRange.ExpandToInclude( (worldBounds.maxs.z - startPosition.z) * zDeltaT );

	// Set the mutual from ranges of xT, yT, zT
	FloatRange xyzMutualOverlapRange( xTRange );

	// Mins
	if( yTRange.min > xyzMutualOverlapRange.min )
		xyzMutualOverlapRange.min = yTRange.min;
	if( zTRange.min > xyzMutualOverlapRange.min )
		xyzMutualOverlapRange.min = zTRange.min;

	// Maxs
	if( yTRange.max < xyzMutualOverlapRange.max )
		xyzMutualOverlapRange.max = yTRange.max;
	if( zTRange.max < xyzMutualOverlapRange.max )
		xyzMutualOverlapRange.max = zTRange.max;

	// If there is no mutual overlap of "t" => Raycast doesn't go through the bounds
	if( xyzMutualOverlapRange.IsValid() == false )
		return hitResult;

	// Raycast goes through the building
	float const tFirstContact = xyzMutualOverlapRange.min;

	if( tFirstContact > 1.f || tFirstContact < 0.f )
		return hitResult;

	// Hit Result at impact point!
	hitResult.didImpact = true;
	hitResult.fractionTravelled = tFirstContact;
	hitResult.impactNormal = Vector3::ZERO;
	hitResult.impactPosition = startPosition + (rayDispacement * tFirstContact);

	return hitResult;
}

Vector3 MagicBlock::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	AABB3	worldBounds				= AABB3( m_transform.GetWorldPosition(), m_size.x, m_size.y, m_size.z );
	Vector3 closestPointInBounds	= worldBounds.GetClosestPointInsideBounds( center );
	Vector3 pushDirection			= center - closestPointInBounds;
	float	pushDistance			= radius - pushDirection.NormalizeAndGetLength();

	// Is Colliding ?
	if( pushDistance > 0.f || AreEqualFloats( pushDistance, 0.f, 4 ) )
	{
		// Yes
		outIsColliding = true;

		Vector3 newCenter = center + (pushDirection * pushDistance);
		return  newCenter;
	}
	else
	{
		// No
		outIsColliding = false;

		return center;
	}
}

