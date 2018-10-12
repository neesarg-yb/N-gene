#pragma once
#include "Building.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
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

RaycastResult Building::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance )
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

bool Building::IsPointInside( Vector3 const &position )
{
	return m_worldBounds.IsPointInsideMe( position );
}
