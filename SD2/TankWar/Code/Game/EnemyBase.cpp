#pragma once
#include "EnemyBase.hpp"
#include "Game/Enemy.hpp"
#include "Game/Terrain.hpp"
#include "Game/theGame.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Profiler/Profiler.hpp"

EnemyBase::EnemyBase( Vector2 const &basePositionXZ, Terrain &isInTerrain, int const maxEnemiesCount, float const spawnRate, float const towerHeight )
	: GameObject( GAME_OBJECT_ENEMY_BASE )
	, m_spawnRate( spawnRate )
	, m_xzPosition( basePositionXZ )
	, m_parentTerrain( isInTerrain )
	, m_maxEnemiesCount( maxEnemiesCount )
	, m_towerSize( Vector3( 10.f, towerHeight, 10.f ) )
{
	// Set Transform
	Vector3 xyzPosition	= m_parentTerrain.Get3DCoordinateForMyPositionAt( m_xzPosition, towerHeight * 0.4f );
	m_transform			= Transform( xyzPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Set Renderable
	m_renderable		= new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh*	mesh		= MeshBuilder::CreateCube( m_towerSize, Vector3::ZERO, RGBA_PURPLE_COLOR );
	m_renderable->SetBaseMesh( mesh );

	// Set Material
	Material* material	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_renderable->SetBaseMaterial( material );

	// Set bounds
	m_worldBounds = AABB3( m_transform.GetWorldPosition(), m_towerSize.x, m_towerSize.y, m_towerSize.z );

	// Set Health
	m_health = 10.f;
}

EnemyBase::~EnemyBase()
{
	delete m_renderable;
}

void EnemyBase::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// If all spawned enemies are alive ( on max capacity ), return
	if( m_currentlyAliveSpawnedEnemies >= m_maxEnemiesCount )
		return;
	
	// Update time for spawn rate
	m_timeSinceLastEnemySpawned += deltaSeconds;

	// If it is not time to spawn a new enemy, return
	if( m_timeSinceLastEnemySpawned < ( 1.f / m_spawnRate ) )
		return;

	// Spawn the enemy
	Enemy* selfExplodingChild = new Enemy( m_xzPosition, m_parentTerrain, *this );
	g_theGame->m_currentBattle->AddNewGameObject( *selfExplodingChild );

	m_timeSinceLastEnemySpawned = 0.f;
}

void EnemyBase::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

void EnemyBase::RemoveRenderablesFromScene( Scene &activeScene )
{
	activeScene.RemoveRenderable( *m_renderable );
}
