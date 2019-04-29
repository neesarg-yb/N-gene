#pragma once
#include "Building.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/GameCommon.hpp"

Building::Building( Vector2 positionXZ, float const height, float const width, Terrain const &parentTerrain, float const offsetFromGround /*= 0.f*/ )
	: m_parentTerrain( parentTerrain )
	, m_size( width, height, width )
{
	// Set Transform
	Vector3 xyzPosition = m_parentTerrain.Get3DCoordinateForMyPositionAt( positionXZ, (height * 0.4f) + offsetFromGround );
	m_transform			= Transform( xyzPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Set Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *mesh = MeshBuilder::CreateCube( m_size, Vector3::ZERO, RGBA_GRAY_COLOR );
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material *material = Material::CreateNewFromFile( "Data\\Materials\\building.material" );
	m_renderable->SetBaseMaterial( material );

	// Set bounds
	m_worldBounds = AABB3( m_transform.GetWorldPosition(), m_size.x, m_size.y, m_size.z );
}

Building::~Building()
{
	delete m_renderable;
	m_renderable = nullptr;
}

void Building::Update( float deltaSeconds )
{
	GameObject::Update( deltaSeconds );
}

void Building::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void Building::RemoveRenderablesFromScene( Scene &activeScene )
{
	activeScene.RemoveRenderable( *m_renderable );
}

bool Building::IsPointInside( Vector3 const &position ) const
{
	return m_worldBounds.IsPointInsideMe( position );
}

RaycastResult Building::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const
{
	PROFILE_SCOPE_FUNCTION();

	// Logic:
	//
	// For each step, until we reach the max distance
	//	If point inside,
	//		*Do the Ray March ( For now, I'm not doing it )

	// Ray
	Ray3 ray = Ray3( startPosition, direction );

	Vector3	position;
	bool	didImpact = false;
	for( float t = 0.f; (t <= maxDistance - accuracy) && (didImpact == false); t += accuracy )
	{
		position	= ray.Evaluate( t );
		didImpact	= IsPointInside( position );
		position	= ray.Evaluate( t + accuracy );	// To make sure that the position we suggest is on the other side of the startPosition
	}

	if( didImpact == false )
		return RaycastResult( position );
	else
	{
		float	distTravelled		= (position - startPosition).GetLength();
		float	fractionTravelled	= distTravelled / maxDistance;
		Vector3	normal				= Vector3::ZERO;

		return RaycastResult( position, normal, fractionTravelled );
	}
}

RaycastResult Building::DoPerfectRaycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const
{
	PROFILE_SCOPE_FUNCTION();

	Vector3	const rayDispacement = direction * maxDistance;
	Vector3	const rayEndPosition = startPosition + rayDispacement;
	RaycastResult hitResult		 = RaycastResult( rayEndPosition );			// Set up as if "NO IMPACT"

	// How much it costs to move on the axis, in terms of parametric "t"
	float const xDeltaT = IsNearZero(rayDispacement.x) ? GetSignedFloatMax(rayDispacement.x) : (1.f / rayDispacement.x);
	float const yDeltaT = IsNearZero(rayDispacement.y) ? GetSignedFloatMax(rayDispacement.y) : (1.f / rayDispacement.y);
	float const zDeltaT = IsNearZero(rayDispacement.z) ? GetSignedFloatMax(rayDispacement.z) : (1.f / rayDispacement.z);
	
	// What is range of "t" when the raycast is inside the building?
	FloatRange xTRange;
	xTRange.ExpandToInclude( (m_worldBounds.mins.x - startPosition.x) * xDeltaT );
	xTRange.ExpandToInclude( (m_worldBounds.maxs.x - startPosition.x) * xDeltaT );

	FloatRange yTRange;
	yTRange.ExpandToInclude( (m_worldBounds.mins.y - startPosition.y) * yDeltaT );
	yTRange.ExpandToInclude( (m_worldBounds.maxs.y - startPosition.y) * yDeltaT );

	FloatRange zTRange;
	zTRange.ExpandToInclude( (m_worldBounds.mins.z - startPosition.z) * zDeltaT );
	zTRange.ExpandToInclude( (m_worldBounds.maxs.z - startPosition.z) * zDeltaT );
	
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
	float const tFirstContact	= xyzMutualOverlapRange.min;

	if( tFirstContact > 1.f || tFirstContact < 0.f )
		return hitResult;

	// Hit Result at impact point!
	hitResult.didImpact			= true;
	hitResult.fractionTravelled	= tFirstContact;
	hitResult.impactNormal		= Vector3::ZERO;
	hitResult.impactPosition	= startPosition + (rayDispacement * tFirstContact);

	return hitResult;
}

Vector3 Building::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	Vector3 closestPointInBounds = m_worldBounds.GetClosestPointInsideBounds( center );
	Vector3 pushDirection		 = center - closestPointInBounds;
	float	pushDistance		 = radius - pushDirection.NormalizeAndGetLength();

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
