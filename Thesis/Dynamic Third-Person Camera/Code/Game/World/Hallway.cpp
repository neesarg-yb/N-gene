#pragma once
#include "Hallway.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/World/Terrain.hpp"

Hallway::Hallway( Vector2 positionXZ, float const height, float const width, float const entryLength, float const exitLength, float const wallThickness, Terrain const &parentTerrain )
	: m_height( height )
	, m_width( width )
	, m_entryLength( entryLength )
	, m_exitLength( exitLength )
	, m_parentTerrain( parentTerrain )
{
	UNUSED( wallThickness );

	// Set Transform
	Vector3 xyzPosition = m_parentTerrain.Get3DCoordinateForMyPositionAt( positionXZ, 0.f );
	m_transform = Transform( xyzPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Construct bounds
	AABB3 entryRightWall, entryLeftWall;
	AABB3 exitRightWall, exitLeftWall;

	// Y|           
	//  |           
	//  |  / Z      
	//  | /         
	//  |/_________X

	float const halfEntryLength		= m_entryLength * 0.5f;
	float const halfExitLength		= m_exitLength * 0.5f;
	float const halfHeight			= m_height * 0.5f;
	float const halfWidth			= m_width * 0.5f;
	float const halfWallThickness	= wallThickness * 0.5f;

	Vector3 centerOfLeftWall	= Vector3( -halfEntryLength, halfHeight, -(halfWidth + halfWallThickness) );
	Vector3 centerOfRightWall	= centerOfLeftWall + Vector3( -halfWidth, 0.f, width + halfWallThickness);
	entryLeftWall	= AABB3( centerOfLeftWall, entryLength, height, wallThickness );
	entryRightWall	= AABB3( centerOfRightWall, entryLength + width, height, wallThickness );
	
	Vector3 centerOfExitLeftWall	= centerOfLeftWall + Vector3( -halfEntryLength, 0.f, -halfExitLength );
	Vector3 centerOfExitRightWall	= centerOfRightWall + Vector3( -(halfEntryLength + halfWidth), 0.f, -(halfExitLength + halfWidth));
	exitLeftWall	= AABB3( centerOfExitLeftWall, wallThickness, height, exitLength );
	exitRightWall	= AABB3( centerOfExitRightWall, wallThickness, height, exitLength + width );
	
	// Mesh builder
	MeshBuilder mb;
	mb.AddCube( entryRightWall.GetSize(),	entryRightWall.GetCenter(),	RGBA_ORANGE_COLOR );
	mb.AddCube( entryLeftWall.GetSize(),	entryLeftWall.GetCenter(),	RGBA_ORANGE_COLOR );
	mb.AddCube( exitRightWall.GetSize(),	exitRightWall.GetCenter(),	RGBA_ORANGE_COLOR );
	mb.AddCube( exitLeftWall.GetSize(),		exitLeftWall.GetCenter(),	RGBA_ORANGE_COLOR );

	// Set Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *mesh = mb.ConstructMesh< Vertex_Lit >();
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material *material = Material::CreateNewFromFile( "Data\\Materials\\building.material" );
	m_renderable->SetBaseMaterial( material );

	// Set bounds
	entryRightWall.TranslateBy( xyzPosition );
	entryLeftWall.TranslateBy( xyzPosition );
	exitRightWall.TranslateBy( xyzPosition );
	exitLeftWall.TranslateBy( xyzPosition );

	m_wallsWorldBounds.push_back( entryRightWall );
	m_wallsWorldBounds.push_back( entryLeftWall );
	m_wallsWorldBounds.push_back( exitRightWall );
	m_wallsWorldBounds.push_back( exitLeftWall );
}

Hallway::~Hallway()
{
	delete m_renderable;
	m_renderable = nullptr;
}

void Hallway::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Hallway::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void Hallway::RemoveRenderablesFromScene( Scene &activeScene )
{
	activeScene.RemoveRenderable( *m_renderable );
}

RaycastResult Hallway::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const
{
	PROFILE_SCOPE_FUNCTION();

	// Ray
	Ray3 ray = Ray3( startPosition, direction );
	RaycastResult result( startPosition + (direction * maxDistance) );

	for each (AABB3 worldBounds in m_wallsWorldBounds)
	{
		DebugRenderWireCube( 0.f, worldBounds.mins, worldBounds.maxs, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

		Vector3	position;
		bool	didImpact = false;
		for( float t = 0.f; (t <= maxDistance - accuracy) && (didImpact == false); t += accuracy )
		{
			position = ray.Evaluate( t );
			didImpact = worldBounds.IsPointInsideMe( position );
			position = ray.Evaluate( t + accuracy );	// To make sure that the position we suggest is on the other side of the startPosition
		}

		if( didImpact )
		{
			float	distTravelled = (position - startPosition).GetLength();
			float	fractionTravelled = distTravelled / maxDistance;
			Vector3	normal = Vector3::ZERO;

			if( fractionTravelled < result.fractionTravelled )
				result = RaycastResult( position, normal, fractionTravelled );
		}
	}

	return result;
}

Vector3 Hallway::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	outIsColliding = false;
	Vector3 newCenterPos = center;

	for( int b = 0; b < m_wallsWorldBounds.size(); b++ )
	{
		AABB3 const &thisWallBounds = m_wallsWorldBounds[b];

		Vector3	closestPointInBounds = thisWallBounds.GetClosestPointInsideBounds( newCenterPos );
		Vector3	pushDirection = newCenterPos - closestPointInBounds;
		float	pushDistance = radius - pushDirection.NormalizeAndGetLength();

		// Is Colliding ?
		if( pushDistance > 0.f || AreEqualFloats( pushDistance, 0.f, 4 ) )
		{
			// Yes
			outIsColliding = true;

			newCenterPos += pushDirection * pushDistance;
		}
		else
		{
			// No
			continue;
		}
	}

	return newCenterPos;
}
