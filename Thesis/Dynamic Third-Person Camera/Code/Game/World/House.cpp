#pragma once
#include "House.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/World/Terrain.hpp"

House::House( Vector2 positionXZ, float const height, float const width, float const length, float const wallThickness, Terrain const &parentTerrain )
	: m_parentTerrain( parentTerrain )
	, m_size( width, height, length )
{
	// Set Transform
	Vector3 xyzPosition = m_parentTerrain.Get3DCoordinateForMyPositionAt( positionXZ, 0.f );
	m_transform			= Transform( xyzPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Construct local bounds
	AABB3 ceilingBounds, leftWallBounds, rightWallBounds, backWallBounds;	// All the walls
	AABB3 frontLeftBounds, frontRightBounds, frontUpBounds;					// Front with the door

	float halfWidth			= width * 0.5f;
	float halfHeight		= height * 0.5f;
	float halfLength		= length * 0.5f;
	float doorWidth			= width * 0.3f;
	float doorHeight		= height * 0.4f;
	float halfDoorWidth		= doorWidth * 0.5f;

	ceilingBounds		= AABB3( Vector3(0.f, height, 0.f), width, wallThickness, length );
	leftWallBounds		= AABB3( Vector3(-halfWidth, halfHeight, 0.f), wallThickness, height, length );
	rightWallBounds		= AABB3( Vector3(+halfWidth, halfHeight, 0.f), wallThickness, height, length );
	backWallBounds		= AABB3( Vector3(0.f, halfHeight, halfLength), width, height, wallThickness );

	frontUpBounds		= AABB3( Vector3(0.f, doorHeight + ((height - doorHeight)*0.5f), -halfLength), width, height - doorHeight, wallThickness ); 
	frontLeftBounds		= AABB3( Vector3( -halfWidth + ((halfWidth - halfDoorWidth)*0.5f), halfHeight, -halfLength), (halfWidth - halfDoorWidth), height, wallThickness );
	frontRightBounds	= AABB3( Vector3( +halfWidth - ((halfWidth - halfDoorWidth)*0.5f), halfHeight, -halfLength), (halfWidth - halfDoorWidth), height, wallThickness );

	// Mesh builder
	MeshBuilder mb;
	mb.AddCube( ceilingBounds.GetSize(),	ceilingBounds.GetCenter(),		RGBA_GRAY_COLOR );
	mb.AddCube( leftWallBounds.GetSize(),	leftWallBounds.GetCenter(),		RGBA_GRAY_COLOR );
	mb.AddCube( rightWallBounds.GetSize(),	rightWallBounds.GetCenter(),	RGBA_GRAY_COLOR );
	mb.AddCube( backWallBounds.GetSize(),	backWallBounds.GetCenter(),		RGBA_GRAY_COLOR );

	mb.AddCube( frontLeftBounds.GetSize(),	frontLeftBounds.GetCenter(),	RGBA_GRAY_COLOR );
	mb.AddCube( frontRightBounds.GetSize(),	frontRightBounds.GetCenter(),	RGBA_GRAY_COLOR );
	mb.AddCube( frontUpBounds.GetSize(),	frontUpBounds.GetCenter(),		RGBA_GRAY_COLOR );

	// Set Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *mesh = mb.ConstructMesh< Vertex_Lit >();
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material *material = Material::CreateNewFromFile( "Data\\Materials\\building.material" );
	m_renderable->SetBaseMaterial( material );

	// Translate bounds to world position
	ceilingBounds.TranslateBy( xyzPosition );
	leftWallBounds.TranslateBy( xyzPosition );
	rightWallBounds.TranslateBy( xyzPosition );
	backWallBounds.TranslateBy( xyzPosition );

	frontLeftBounds.TranslateBy( xyzPosition );
	frontRightBounds.TranslateBy( xyzPosition );
	frontUpBounds.TranslateBy( xyzPosition );

	// Set bounds
	m_wallsWorldBounds.push_back( ceilingBounds );
	m_wallsWorldBounds.push_back( leftWallBounds );
	m_wallsWorldBounds.push_back( rightWallBounds );
	m_wallsWorldBounds.push_back( backWallBounds );

	m_wallsWorldBounds.push_back( frontLeftBounds );
	m_wallsWorldBounds.push_back( frontRightBounds );
	m_wallsWorldBounds.push_back( frontUpBounds );
}

House::~House()
{
	delete m_renderable;
	m_renderable = nullptr;
}

void House::Update( float deltaSeconds )
{
	GameObject::Update( deltaSeconds );
}

void House::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void House::RemoveRenderablesFromScene( Scene &activeScene )
{
	activeScene.RemoveRenderable( *m_renderable );
}

RaycastResult House::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance, float accuracy ) const
{
	PROFILE_SCOPE_FUNCTION();

	// Ray
	Ray3 ray = Ray3( startPosition, direction );
	RaycastResult result( startPosition + (direction * maxDistance) );

	for each (AABB3 worldBounds in m_wallsWorldBounds)
	{
		Vector3	position;
		bool	didImpact = false;
		for( float t = 0.f; (t <= maxDistance - accuracy) && (didImpact == false); t += accuracy )
		{
			position	= ray.Evaluate( t );
			didImpact	= worldBounds.IsPointInsideMe( position );
			position	= ray.Evaluate( t + accuracy );	// To make sure that the position we suggest is on the other side of the startPosition
		}

		if( didImpact )
		{
			float	distTravelled		= (position - startPosition).GetLength();
			float	fractionTravelled	= distTravelled / maxDistance;
			Vector3	normal				= Vector3::ZERO;

			if( fractionTravelled < result.fractionTravelled)
				result = RaycastResult( position, normal, fractionTravelled );
		}
	}

	return result;
}

Vector3 House::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	UNUSED( radius );
	TODO( "Write a collision check!" );

	outIsColliding = false;
	return center;
}
