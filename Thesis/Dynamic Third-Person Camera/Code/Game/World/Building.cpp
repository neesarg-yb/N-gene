#pragma once
#include "Building.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/World/Terrain.hpp"

Building::Building( Vector2 positionXZ, float const height, float const width, Terrain const &parentTerrain )
	: m_parentTerrain( parentTerrain )
	, m_size( width, height, width )
{
	// Set Transform
	Vector3 xyzPosition = m_parentTerrain.Get3DCoordinateForMyPositionAt( positionXZ, height * 0.4f );
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

void Building::PushTheSphereOutOfTheBuilding( Sphere &sphereInsideBuilding ) const
{
	Vector3	&center = sphereInsideBuilding.center;
	float	&radius = sphereInsideBuilding.radius;

	// Get six planes of the AABB
	Plane3 allPlanes[6];
	m_worldBounds.GetSixPlanes( allPlanes );

	// Which one is the closest plane?
	int		closestPlaneIdx			= 0;
	float	closestPlaneDistance	= allPlanes[0].GetDistanceFromPoint( center );

	for( int i = 1; i < 6; i++ )
	{
		float distanceFromPlane = allPlanes[i].GetDistanceFromPoint( center );
		if( distanceFromPlane > closestPlaneDistance )	// Because both will be negative, so the bigger is smaller
		{
			closestPlaneIdx			= i;
			closestPlaneDistance	= distanceFromPlane;
		}
	}
	
	// Push the center: in the direction of normal of the closest plane
	Plane3 &closestPlane = allPlanes[ closestPlaneIdx ];
	DebugRenderLineSegment( 20.f, sphereInsideBuilding.center, RGBA_WHITE_COLOR, sphereInsideBuilding.center + (closestPlane.normal * -1.f * closestPlaneDistance), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	center += closestPlane.normal * (-1.f * closestPlaneDistance);

 
 	// Debug Render
// 	DebugRenderLineSegment( 0.f, m_worldBounds.mins, RGBA_WHITE_COLOR, m_worldBounds.mins + (allPlanes[0].normal * 10.f), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 	DebugRenderLineSegment( 0.f, m_worldBounds.mins, RGBA_WHITE_COLOR, m_worldBounds.mins + (allPlanes[1].normal * 10.f), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 	DebugRenderLineSegment( 0.f, m_worldBounds.mins, RGBA_WHITE_COLOR, m_worldBounds.mins + (allPlanes[2].normal * 10.f), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 
// 	DebugRenderLineSegment( 0.f, m_worldBounds.maxs, RGBA_WHITE_COLOR, m_worldBounds.maxs + (allPlanes[3].normal * 10.f), RGBA_GREEN_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 	DebugRenderLineSegment( 0.f, m_worldBounds.maxs, RGBA_WHITE_COLOR, m_worldBounds.maxs + (allPlanes[4].normal * 10.f), RGBA_GREEN_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 	DebugRenderLineSegment( 0.f, m_worldBounds.maxs, RGBA_WHITE_COLOR, m_worldBounds.maxs + (allPlanes[5].normal * 10.f), RGBA_GREEN_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 
// 	DebugRenderSphere( 0.f, m_worldBounds.mins, 1.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
}

RaycastResult Building::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const
{
	float const sampleSize = 0.2f;
	// Logic:
	//
	// For each step, until we reach the max distance
	//	If point inside,
	//		*Do the Ray March ( For now, I'm not doing it )

	// Ray
	Ray3 ray = Ray3( startPosition, direction );

	Vector3	position;
	bool	didImpact = false;
	for( float t = 0.f; (t <= maxDistance - sampleSize) && (didImpact == false); t += sampleSize )
	{
		position	= ray.Evaluate( t );
		didImpact	= IsPointInside( position );
		position	= ray.Evaluate( t + sampleSize );	// To make sure that the position we suggest is on the other side of the startPosition
	}

	if( didImpact == false )
		return RaycastResult( startPosition );
	else
	{
		float	distTravelled		= (position - startPosition).GetLength();
		float	fractionTravelled	= distTravelled / maxDistance;
		Vector3	normal				= Vector3::ZERO;

		return RaycastResult( position, normal, fractionTravelled );
	}
}

Vector3 Building::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	// Increase the AABB3's borders such that the center touches them first, if it is colliding
	float distanceFromBuilding = m_worldBounds.GetDistanceFromPoint( center ) - radius;

//	if( distanceFromBuilding > 0.f )
//		DebugRenderSphere( 0.f, center, radius, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
//	else
//		DebugRenderSphere( 0.f, center, radius, RGBA_RED_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	// Is Colliding ?
	if( distanceFromBuilding > 0.f )
	{
		outIsColliding = false;
		return center;				// return, if point is not inside
	}
	else
		outIsColliding = true;

	// Sphere is inside, push it out
	Sphere collidingSphere( center, radius );
	PushTheSphereOutOfTheBuilding( collidingSphere );
	
//	return center;
	return collidingSphere.center;
}
