#pragma once
#include "Battle.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Game/Terrain.hpp"
#include "Game/Turret.hpp"
#include "Game/Tank.hpp"

using namespace tinyxml2;

Scene*				  Battle::s_battleScene;
Camera*				  Battle::s_camera;
std::vector< Light* > Battle::s_lightSources;

void Battle::AddNewPointLightToCamareaPosition( Rgba lightColor )
{
	Vector3 cameraPos = s_camera->m_cameraTransform.GetWorldPosition();
	
	Light* newLight	= new Light( cameraPos, s_camera->m_cameraTransform.GetRotation() );
	newLight->SetUpForPointLight( 40.f, Vector3( 0.f, 0.f, 1.f ), lightColor );
	s_lightSources.push_back( newLight );

	s_battleScene->AddLight( *newLight );
	s_battleScene->AddRenderable( *newLight->m_renderable );

	// DebugRenderWireSphere( 10.f, cameraPos, .5f, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderSphere( 10.f, cameraPos, .5f, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderQuad( 10.f, cameraPos, Vector3( 15.f, 45.f, 15.f ), Vector2::ONE_ONE, nullptr, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderWireCube( 10.f, cameraPos - Vector3( 0.5f, 0.5f, 0.5f ), cameraPos + Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderBasis( 10.f, newLight->m_renderable->GetTransform().GetWorldTransformMatrix(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderLineSegment( 10.f, cameraPos - Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, cameraPos + Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderPoint( 10.f, cameraPos - Vector3( 0.f, 0.f, -.5f ), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRender2DText( 2.5f, Vector2::ZERO, 15.f, RGBA_WHITE_COLOR, RGBA_BLACK_COLOR, "Light Spawned!" );
	// DebugRender2DLine( 1.f, Vector2( -100.f, -100.f), RGBA_WHITE_COLOR, Vector2( 100.f, 100.f ), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
	// DebugRender2DLine( 1.f, Vector2( -100.f, 100.f), RGBA_WHITE_COLOR, Vector2( 100.f, -100.f ), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
	// DebugRender2DQuad( 2.5f, AABB2( Vector2::ZERO , 10.f, 10.f), RGBA_WHITE_COLOR, RGBA_PURPLE_COLOR );
}

void Battle::AddNewGameObject( GameObject &newGO )
{
	// Add GameObject to pool
	m_allGameObjects[ newGO.m_type ].push_back( &newGO );

	// Add its Renderable to scene
	newGO.AddRenderablesToScene( *s_battleScene );
}

Battle::Battle()
{
	
}

Battle::~Battle()
{
	delete m_renderingPath;
	delete s_battleScene;
	
	DebugRendererShutdown();

	// Lights
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		delete s_lightSources[i];

	// GameObject Pool
	for( int type = 0; type < NUM_GAME_OBJECT_TYPES; type++ )
	{
		for( unsigned int i = 0; i < m_allGameObjects[ type ].size(); i++ )
			delete m_allGameObjects[ type ][i];

		m_allGameObjects[ type ].clear();
	}
	
	delete s_camera;
}

void Battle::Startup()
{
	// Setup the camera
	s_camera = new Camera();
	s_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	s_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() ); 
	s_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f );
	s_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	s_lightSources.push_back( new Light( Vector3::ZERO ) );
	s_lightSources[0]->SetUpForPointLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	s_lightSources[0]->EnablePerimeterRotationAround( Vector3( 0.f, 0.f, 5.f ), 10.f );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, s_camera );

	// Battle Scene
	s_battleScene = new Scene();

	s_battleScene->AddLight( *s_lightSources[0] );
	s_battleScene->AddRenderable( *s_lightSources[0]->m_renderable );

	s_battleScene->AddCamera( *s_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );

	// TERRAIN
	m_terrain = new Terrain( Vector3( 0.f, 0.f, 0.f ), IntVector2( 500, 400 ), 30.f, "Data\\Images\\terrain\\heightmapt.png" );
	AddNewGameObject( *m_terrain );

	// PLAYER TANK
	m_playerTank = new Tank( Vector2::ZERO, *m_terrain, true, s_camera );
	s_lightSources[0]->m_transform.SetParentAs( &s_camera->m_cameraTransform );
	AddNewGameObject( *m_playerTank );
}

void Battle::BeginFrame()
{

}

void Battle::EndFrame()
{

}

void Battle::Update( float deltaSeconds )
{
	// Battle::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;
	

	// Lights
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		s_lightSources[i]->Update( deltaSeconds );


	// For each type
	for ( int type = 0; type < NUM_GAME_OBJECT_TYPES; type++ )
	{
		// For each game objects
		for( uint idx = 0; idx < m_allGameObjects[ type ].size(); idx++ )
			m_allGameObjects[ type ][ idx ]->Update( deltaSeconds );
	}

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );
	

	// Spawn Lights according to input
	if( g_theInput->WasKeyJustPressed( 'R' ) )
		AddNewPointLightToCamareaPosition( RGBA_RED_COLOR );
	if( g_theInput->WasKeyJustPressed( 'G' ) )
		AddNewPointLightToCamareaPosition( RGBA_GREEN_COLOR );
	if( g_theInput->WasKeyJustPressed( 'B' ) )
		AddNewPointLightToCamareaPosition( RGBA_BLUE_COLOR );
	if( g_theInput->WasKeyJustPressed( 'W' ) )
		AddNewPointLightToCamareaPosition( RGBA_WHITE_COLOR );

}

void Battle::Render() const
{
	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", s_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *s_camera, *s_battleScene );
	
	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) + " [ UP, DOWN ]" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);
	std::string toSpawnSpotLights	= std::string( "Spawn new SpotLights," );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);
	toSpawnSpotLights	= std::string( "Keys: R(red), G(green), B(blue), W(white)" );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);

	DebugRendererRender();
}

double Battle::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Battle::RotateTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const rotationSpeed = 45.f;	// Degrees per seconds

	XboxController &inputController	= g_theInput->m_controller[0];
	Vector2 axisChange				= inputController.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	Vector2 finalYXEulerRotation	= axisChange * rotationSpeed * deltaSeconds;

	TODO("I'm doing Anti-Clockwise rotation, under the hood. But it seems the camera is doing Clockwise rotation.. :/");
	s_camera->RotateCameraBy( Vector3( -finalYXEulerRotation.y, finalYXEulerRotation.x, 0.f ) );
}

void Battle::ChnageLightAsPerInput(float deltaSeconds)
{
	// Ambient Light
	static float const ambientFactorChangeSpeed = 0.35f;		// Per seconds

	if (g_theInput->IsKeyPressed(UP))
	{
		m_ambientLight.w += ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}
	if (g_theInput->IsKeyPressed(DOWN))
	{
		m_ambientLight.w -= ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}

	g_theRenderer->SetAmbientLight( m_ambientLight );
}