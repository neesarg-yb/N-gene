#pragma once
#include "Tank.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Terrain.hpp"
#include "Engine/Core/StringUtils.hpp"

Tank::Tank( Vector3 const &spawnPosition, Terrain &isInTerrain )
	: m_parentTerrain( isInTerrain )
{
	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );
	m_renderable = new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *sphereMesh = MeshBuilder::CreateSphere( m_radius, 30, 30 );
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
	Vector3 finalPosition		= currentPosition + worldTranslation;

 	Vector2 finalXZPosition		= Vector2( finalPosition.x, finalPosition.z );
 	finalPosition.y				= m_parentTerrain.GetYCoordinateForMyPositionAt( finalXZPosition );
	finalPosition.y				+= m_radius;
	m_transform.SetPosition( finalPosition );

	// Right Stick
	Vector2 rightStickNormalized = thecontroller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	float	yRotation			 = rightStickNormalized.x * m_rotationSpeed * deltaSeconds;
	float	xRotation			 = rightStickNormalized.y * m_rotationSpeed * deltaSeconds;
	Vector3 currentRotation		 = m_transform.GetRotation();

	m_transform.SetRotation( currentRotation + Vector3( xRotation, yRotation, 0.f ) );

	// Debug Trail
	static float remainingTrailTime = m_spawnTrailPointAfter;
	remainingTrailTime -= deltaSeconds;

	if( remainingTrailTime <= 0 )
	{
		DebugRenderPoint( 10.f, 0.5f, m_transform.GetWorldPosition(), RGBA_YELLOW_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
		remainingTrailTime = m_spawnTrailPointAfter;
	}

// 	DebugRenderPoint( 0.f, 3.f, Vector3::ZERO, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
// 	std::string pos  = Stringf("Pos: ( %f, %f, %f )", m_transform.GetWorldPosition().x,m_transform.GetWorldPosition().y,m_transform.GetWorldPosition().z );
// 
// 	DebugRender2DText( 0.f, Vector2(0.f, 0.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, pos.c_str() );
}
