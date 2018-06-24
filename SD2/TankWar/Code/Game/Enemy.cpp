#pragma once
#include "Enemy.hpp"
#include "Game/Terrain.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Scene.hpp"

Enemy::Enemy( Vector2 const &spawnPosition, Terrain &isInTerrain )
	: GameObject( GAME_OBJECT_ENEMY )
	, m_paerntTerrain( isInTerrain )
	, m_currentPositionXZ( spawnPosition )
{
	// Set Transform
	Vector3 xyzPos		= m_paerntTerrain.Get3DCoordinateForMyPositionAt( m_currentPositionXZ, m_radius );
	Vector3 rotation	= Get3DRotation( m_forwardDiractionXZ );
	m_transform			= Transform( xyzPos, rotation, Vector3::ONE_ALL );

	// Set Renderable
	m_renderable		= new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	MeshBuilder	mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddSphere( m_radius,			10, 10, Vector3::ZERO * m_radius,					RGBA_BLUE_COLOR );
	mb.AddSphere( m_radius * 0.2f,	10, 10, Vector3(  0.2f, 0.4f, 0.8f ) * m_radius,	RGBA_WHITE_COLOR );
	mb.AddSphere( m_radius * 0.2f,	10, 10, Vector3( -0.2f, 0.4f, 0.8f ) * m_radius,	RGBA_WHITE_COLOR );
	mb.End();
	Mesh* mesh = mb.ConstructMesh<Vertex_Lit>();
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material* material = Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_renderable->SetBaseMaterial( material );
}

Enemy::~Enemy()
{

}

void Enemy::Update( float deltaSeconds )
{
	// Rotation Test
	m_forwardDiractionXZ.RotateByDegrees( m_rotationSpeed * deltaSeconds );
	Vector3 rotation = Get3DRotation( m_forwardDiractionXZ );
	m_transform.SetRotation( rotation );
}

void Enemy::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

Vector3 Enemy::Get3DRotation( Vector2 xzForwardDirection )
{
	float orientationInDegreesFromXAxis = xzForwardDirection.GetOrientationDegrees();
	float orientationInDegreesFromZAxis = orientationInDegreesFromXAxis + 90.f;

	return Vector3( 0.f, orientationInDegreesFromZAxis, 0.f );
}
