#pragma once
#include "Hallway.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
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

	// Set Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *mesh = MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3::ZERO, RGBA_ORANGE_COLOR );
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material *material = Material::CreateNewFromFile( "Data\\Materials\\building.material" );
	m_renderable->SetBaseMaterial( material );

	// Set bounds
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
	UNUSED( accuracy );

	return RaycastResult( startPosition + (direction * maxDistance) );
}

Vector3 Hallway::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	UNUSED( radius );

	outIsColliding = false;
	return center;
}
