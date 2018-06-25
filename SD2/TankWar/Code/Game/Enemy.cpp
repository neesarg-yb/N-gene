#pragma once
#include "Enemy.hpp"
#include "Game/theGame.hpp"
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
	m_transform			= Transform( xyzPos, Vector3::ZERO, Vector3::ONE_ALL );

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
	//////////////////////
	//  PHYSICS UPDATE  //
	//////////////////////

	// Normalize current velocity
	m_currentVelocityXZ	= m_currentVelocityXZ.GetNormalized();

	// Set facing rotation according to velocity
	Vector3 rotation	= Get3DRotation( m_currentVelocityXZ );
	m_transform.SetRotation( rotation );

	// Change position according to velocity
	m_currentPositionXZ	= m_currentPositionXZ + ( m_currentVelocityXZ * m_speed * deltaSeconds );
	Vector3 newPosition = m_paerntTerrain.Get3DCoordinateForMyPositionAt( m_currentPositionXZ );
	m_transform.SetPosition( newPosition );


	//////////////////////
	// BEHAVIOUR UPDATE //
	//////////////////////

	// Move Towards player
	Vector2 playerPosXZ	= g_theGame->m_currentBattle->m_playerTank->m_xzPosition;
	SeekTowards( playerPosXZ );
}

void Enemy::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void Enemy::AddToVelocity( Vector2 const &velToAdd )
{
	m_currentVelocityXZ += velToAdd;
}

void Enemy::SeekTowards( Vector2 const &targetPos )
{
	Vector2 desiredVelocity		= (targetPos - m_currentPositionXZ).GetNormalized();
	Vector2 steeringVelocity	= desiredVelocity - m_currentVelocityXZ;						// Note: Assumes that m_currentVelocityXZ is already NORMALIZED

	AddToVelocity( steeringVelocity );
}

Vector3 Enemy::Get3DRotation( Vector2 xzForwardDirection )
{
	float orientationInDegreesFromXAxis = -1.f * xzForwardDirection.GetOrientationDegrees();	// Get Orientation considers clockwise rotation negative
	float orientationInDegreesFromZAxis = orientationInDegreesFromXAxis + 90.f;

	return Vector3( 0.f, orientationInDegreesFromZAxis, 0.f );
}
