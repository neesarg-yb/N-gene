#pragma once
#include "Player.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/World/Terrain.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
{
	// Setup the Renderable - BB8
//	m_renderable		= new Renderable( Vector3::ZERO, Vector3::ZERO, Vector3( 0.007f, 0.007f, 0.007f ) );
	m_renderable		= new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
/*	bool loadSuccess	= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\bb8\\BB8.obj", *m_renderable );

	// If load fails
	GUARANTEE_RECOVERABLE( loadSuccess, "Failed to load the player model!" );
	if( loadSuccess == false )
	{
		m_renderable = nullptr;
		delete m_renderable;
	}
*/

	Material *newMat	= Material::CreateNewFromFile( "Data\\Materials\\Block_Metal.material" );
	Mesh	 *newMesh	= MeshBuilder::CreateSphere( 0.25f, 10U, 10U );
	m_renderable->SetBaseMaterial( newMat );
	m_renderable->SetBaseMesh( newMesh );
}

Player::~Player()
{
	m_terrain = nullptr;
}

void Player::Update( float deltaSeconds )
{
	// Add Drag
	MotionDragUpdate( deltaSeconds );

	// Updates my velocity according to the input
	MovementUpdate( deltaSeconds );

	// Makes sure that I'm on terrain, not under it
	CheckAndSnapOnTerrainSurface();

	// Updates the position based on velocity
	GameObject::Update( deltaSeconds );
}

void Player::AddRenderablesToScene( Scene &activeScene )
{
	// No renderable? => return
	if( m_renderable == nullptr )
		return;

	activeScene.AddRenderable( *m_renderable );
}

void Player::RemoveRenderablesFromScene( Scene &activeScene )
{
	// No renderable? => return
	if( m_renderable == nullptr )
		return;

	activeScene.RemoveRenderable( *m_renderable );
}

void Player::MotionDragUpdate( float deltaSeonds )
{
	UNUSED( deltaSeonds );
}

void Player::MovementUpdate( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Get input from Xbox Controller
	XboxController &controller			= g_theInput->m_controller[0];
	Vector2			inputDirectionXZ	= controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;

	// Applying Force
	Vector2 forceXZ = inputDirectionXZ * 10.f;
	ApplyForce( forceXZ.x, 0.f, forceXZ.y );
}

void Player::CheckAndSnapOnTerrainSurface()
{

}

// Newton's Second Law
void Player::ApplyForce( float x, float y, float z )
{
	m_acceleration += ( Vector3( x, y, z ) / m_mass );
}
