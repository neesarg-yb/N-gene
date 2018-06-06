#pragma once
#include "Tank.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

Tank::Tank( Vector3 const &spawnPosition )
{
	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );
	m_renderable = new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *sphereMesh = MeshBuilder::CreateSphere( 4.f, 30, 30 );
	m_renderable->SetBaseMesh( sphereMesh );

	// Set Material
	Material *sphereMaterial = Material::CreateNewFromFile( "Data\\Materials\\stone_sphere.material" );
	m_renderable->SetBaseMaterial( sphereMaterial );
}

Tank::~Tank()
{
	delete m_renderable;
}

void Tank::Update( float deltaSeconds )
{
	// Control the Tank using Xbox Controller
	XboxController &thecontroller = g_theInput->m_controller[0];

	// Left Stick
	Vector2 leftStickNormalized = thecontroller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;

	// Move Forward
	Vector2 translationXZ		= leftStickNormalized * m_speed * deltaSeconds;
	Vector3 translation			= Vector3( translationXZ.x, 0.f, translationXZ.y );
	Vector3 worldTranslation	= m_transform.GetWorldTransformMatrix().Multiply( translation, 0.f );
	Vector3 currentPosition		= m_transform.GetPosition();

	m_transform.SetPosition( currentPosition + worldTranslation );

	// Right Stick
	Vector2 rightStickNormalized = thecontroller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	float	zRotation			 = rightStickNormalized.x * m_rotationSpeed * deltaSeconds;
	Vector3 currentRotation		 = m_transform.GetRotation();

	m_transform.SetRotation( currentRotation + Vector3( 0.f, zRotation, 0.f ) );

	// Debug Trail
	static float remainingTrailTime = m_spawnTrailPointAfter;
	remainingTrailTime -= deltaSeconds;

	if( remainingTrailTime <= 0 )
	{
		DebugRenderPoint( 10.f, m_transform.GetWorldPosition(), RGBA_YELLOW_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
		remainingTrailTime = m_spawnTrailPointAfter;
	}
}
