#pragma once
#include "MinecraftWorld.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

MinecraftWorld::MinecraftWorld( Clock const *parentClock, char const *sceneName )
	: GameState( sceneName, parentClock )
{	
	// Setting up the Camera
	m_camera = new MCamera( *g_theRenderer );
	m_camera->m_position = Vector3( -3.f, 3.f, 3.f );
	m_camera->m_yawDegreesAboutZ = -40.f;
	m_camera->SetPitchDegreesAboutY( 25.f );
}

MinecraftWorld::~MinecraftWorld()
{
	// Camera
	delete m_camera;
	m_camera = nullptr;
}

void MinecraftWorld::JustFinishedTransition()
{
	g_theInput->ShowCursor( false );
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
}

void MinecraftWorld::BeginFrame()
{

}

void MinecraftWorld::EndFrame()
{

}

void MinecraftWorld::Update()
{
	PROFILE_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();
	ProcessInput( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
	{
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		g_theInput->ShowCursor( true );

		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	}

	m_camera->RebuildMatrices();
}

void MinecraftWorld::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );
	
	// Camera
	Camera &camera = *m_camera->GetCamera();
	g_theRenderer->BindCamera( &camera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	// Pre Render
	camera.PreRender( *g_theRenderer );
	
	//----------------------------
	// The Rendering starts here..
	//
	RenderBasis( 1.f );
	m_testCube.Render( *g_theRenderer );
	
	// Post Render
	camera.PostRender( *g_theRenderer );
}

void MinecraftWorld::ProcessInput( float deltaSeconds )
{
	// Camera Rotation
	Vector2 mouseChange = g_theInput->GetMouseDelta();
	float const curentCamPitch = m_camera->GetPitchDegreesAboutY();

	m_camera->m_yawDegreesAboutZ	-= mouseChange.x * m_camRotationSpeed;
	m_camera->SetPitchDegreesAboutY( curentCamPitch + (mouseChange.y * m_camRotationSpeed) );

	// Camera Position
	float	const camYaw	 = m_camera->m_yawDegreesAboutZ;
	Vector3 const forwardDir = Vector3( CosDegree(camYaw), SinDegree(camYaw), 0.f );
	Vector3 const leftDir	 = Vector3( forwardDir.y * -1.f, forwardDir.x, 0.f );
	Vector3 const upDir		 = Vector3( 0.f, 0.f, 1.f );

	float forwardMovement = 0.f;
	float leftMovement = 0.f;
	float upMovement = 0.f;

	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= m_flySpeed * deltaSeconds;

	Vector3 positionChange = (forwardDir * forwardMovement) + (leftDir * leftMovement) + (upDir * upMovement);
	m_camera->m_position += positionChange;
}

void MinecraftWorld::RenderBasis( float length ) const
{
	Vertex_3DPCU vBuffer[6];
	
	vBuffer[0].m_color		= RGBA_RED_COLOR;
	vBuffer[0].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[1].m_color		= RGBA_RED_COLOR;
	vBuffer[1].m_position	= Vector3( length, 0.f, 0.f );


	vBuffer[2].m_color		= RGBA_GREEN_COLOR;
	vBuffer[2].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[3].m_color		= RGBA_GREEN_COLOR;
	vBuffer[3].m_position	= Vector3( 0.f, length, 0.f );
	
	vBuffer[4].m_color		= RGBA_BLUE_COLOR;
	vBuffer[4].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[5].m_color		= RGBA_BLUE_COLOR;
	vBuffer[5].m_position	= Vector3( 0.f, 0.f, length );

	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );
}
