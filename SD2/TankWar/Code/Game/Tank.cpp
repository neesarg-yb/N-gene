#pragma once
#include "Tank.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Terrain.hpp"

Tank::Tank( Vector2 const &spawnPosition, Terrain &isInTerrain, bool isPlayer, Camera* attachedCamera )
	: m_parentTerrain( isInTerrain )
	, m_isControlledByXbox( isPlayer )
	, m_attachedCamera( attachedCamera )
{
	// Setup the Camera
	m_attachedCamera->LookAt(	Vector3( 0.f, 7.f, -10.f ),
								Vector3( 0.f, 3.f, 0.f ),
								Vector3::UP );

	// Set transform hierarchy
	//		Anchor Transform							( Sets			  Y-Rotation )
	//			|---> Attached Camera's Transform		( Don't need	 XZ-Rotation )
	//			|---> Tank's Transform					( Sets			 XZ-Rotation ) 
	//					|---> Renderable's Transfomr	( Gets all		XYZ-Rotation )
	
	// Anchor Transform:
	Vector3	xyzPos		= m_parentTerrain.Get3DCoordinateForMyPositionAt( spawnPosition );
	m_anchorTransform	= Transform( xyzPos,		Vector3::ZERO, Vector3::ONE_ALL );
	// Tank's Transform:
	m_transform			= Transform( Vector3::ZERO, Vector3::ZERO, Vector3::ONE_ALL );
	m_transform.SetParentAs( &m_anchorTransform );
	m_renderable		= new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
	// Camera's Transform:
	m_attachedCamera->m_cameraTransform.SetParentAs( &m_anchorTransform );

	// Set Mesh
	Mesh *sphereMesh = MeshBuilder::CreateCube( Vector3( 2.f, m_height, 2.f ), Vector3( 0.f, m_height, 0.f ) );
	m_renderable->SetBaseMesh( sphereMesh );

	// Set Material
	Material *sphereMaterial = Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_renderable->SetBaseMaterial( sphereMaterial );
}

Tank::~Tank()
{
	delete m_renderable;
}

void Tank::Update( float deltaSeconds )
{
	// If Player Tank, handle input
	if( m_isControlledByXbox )
		HandleInput( deltaSeconds );		// m_xzPosition is set

	// Set transform
	Matrix44	tankMatrix		= m_transform.GetWorldTransformMatrix();
	Matrix44	alignmentMatrix = m_parentTerrain.GetModelMatrixForMyPositionAt( m_xzPosition, m_xzForward, m_xzRight );
	Matrix44	lerpAlignMatrix	= Matrix44::LerpMatrix( tankMatrix, alignmentMatrix, deltaSeconds * 8.f );

	Transform	worldTransform;				// It contains everything
	worldTransform.SetFromMatrix( lerpAlignMatrix );
	Vector3		worldRotation	= worldTransform.GetRotation();
	// Anchor Transform just needs Position & Y-Rotation
	m_anchorTransform.SetPosition( worldTransform.GetWorldPosition() );
	m_anchorTransform.SetRotation( Vector3( 0.f, worldRotation.y, 0.f ) );
	// Tank Transform need XZ-Rotation
	worldRotation.y = 0.f;
	m_transform.SetRotation( worldRotation );

	// Debug Trail
	static float remainingTrailTime = m_spawnTrailPointAfter;
	remainingTrailTime -= deltaSeconds;

	if( remainingTrailTime <= 0 )
	{
		DebugRenderPoint( 10.f, 0.5f, m_transform.GetWorldPosition(), RGBA_YELLOW_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
		remainingTrailTime = m_spawnTrailPointAfter;
	}

	// Debug Position
	std::string pos  = Stringf("Pos: ( %f, %f, %f )", m_transform.GetWorldPosition().x,m_transform.GetWorldPosition().y,m_transform.GetWorldPosition().z );
	DebugRender2DText( 0.f, Vector2(0.f, 0.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, pos.c_str() );
}

void Tank::HandleInput( float deltaSeconds )
{
	// Control the Tank using Xbox Controller
	XboxController &thecontroller = g_theInput->m_controller[0];

	// Left Stick
	Vector2 leftStickNormalized = thecontroller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	// Right Stick
	Vector2 rightStickNormalized = thecontroller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	float	yRotation			 = rightStickNormalized.x * m_rotationSpeed * deltaSeconds;

	// Set Forward & Right directions
	m_xzRight.RotateByDegrees( -yRotation );
	m_xzForward.RotateByDegrees( -yRotation );
	DebugRender2DText( 0.f, Vector2( -400.f, -400.f ), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf( "Right:   ( %f, %f ) ", m_xzRight.x, m_xzRight.y ) );
	DebugRender2DText( 0.f, Vector2( -400.f, -380.f ), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf( "Forward: ( %f, %f ) ", m_xzForward.x, m_xzForward.y ) );

	// Move Forward
	Vector2 translationXZ		 = leftStickNormalized * m_speed * deltaSeconds;
	m_xzPosition				+= (m_xzForward * translationXZ.y) + (m_xzRight * translationXZ.x);
}
