#pragma once
#include "Level1.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

Level1::Level1()
	: GameState( "LEVEL 1" )
{
	// Setup the Orbit camera
	m_camera = new OrbitCamera( Vector3::ZERO );
	// Set Color Targets
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	// Projection Matrix
	m_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f ); 
	// Orbit Camera
	m_camera->SetSphericalCoordinate( 12.f, -90.f, 90.f );
	// Skybox
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	m_lightSources.push_back( new Light( Vector3( 0.f, 10.f, -10.f ), Vector3( -45.f, 0.f, 0.f ) ) );
	m_lightSources[0]->SetUpForDirectionalLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	m_lightSources[0]->UsesShadowMap( false );

	// Battle Scene
	m_levelScene = new Scene();
	m_levelScene->AddLight( *m_lightSources[0] );
	m_levelScene->AddRenderable( *m_lightSources[0]->m_renderable );
	m_levelScene->AddCamera( *m_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
}

Level1::~Level1()
{
	delete m_renderingPath;
	delete m_levelScene;

	// Lights
	for( unsigned int i = 0; i < m_lightSources.size(); i++ )
		delete m_lightSources[i];
	m_lightSources.clear();

	// GameObject Pool
	for( unsigned int i = 0; i < m_allGameObjects.size(); i++ )
		delete m_allGameObjects[i];

	m_allGameObjects.clear();

	delete m_camera;
}

void Level1::JustFinishedTransition()
{
	DebugRendererChange3DCamera( m_camera );
}

void Level1::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level1::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level1::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );

	// Level::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Level1::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", m_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_levelScene );

//	// DebugText for Lighting and Shader..
//	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) );
//	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);

	DebugRendererRender();
}
