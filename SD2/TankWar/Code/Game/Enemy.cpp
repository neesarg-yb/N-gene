#pragma once
#include "Enemy.hpp"
#include "Game/Terrain.hpp"
#include "Game/EnemyBase.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Scene.hpp"

Enemy::Enemy( Vector2 const &spawnPosition, Terrain &isInTerrain, EnemyBase &parentBase )
	: GameObject( GAME_OBJECT_ENEMY )
	, m_parentBase( parentBase )
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

	// Tell parent, I AM BORN!
	m_parentBase.IncreaseChildCount();
}

Enemy::~Enemy()
{
	// Tell parent, I'M ABOUT TO DIE..! :(
	m_parentBase.ReduceChildCount();

	delete m_renderable;
}

void Enemy::Update( float deltaSeconds )
{
	// Rotation Test
	m_forwardDiractionXZ.RotateByDegreesClockwise( m_rotationSpeed * deltaSeconds );
	Vector3 rotation = Get3DRotation( m_forwardDiractionXZ );
	m_transform.SetRotation( rotation );

	// Move Forward
	m_currentPositionXZ		= m_currentPositionXZ + ( m_forwardDiractionXZ * ( m_speed * deltaSeconds ) );
	Vector3 xyzPos			= m_paerntTerrain.Get3DCoordinateForMyPositionAt( m_currentPositionXZ, m_radius );
	m_transform.SetPosition( xyzPos );
}

void Enemy::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

Vector3 Enemy::Get3DRotation( Vector2 xzForwardDirection )
{
	float orientationInDegreesFromXAxis = -1.f * xzForwardDirection.GetOrientationDegrees();	// Get Orientation considers clockwise rotation negative
	float orientationInDegreesFromZAxis = orientationInDegreesFromXAxis + 90.f;

	return Vector3( 0.f, orientationInDegreesFromZAxis, 0.f );
}
