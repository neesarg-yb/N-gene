#pragma once
#include "Level.hpp"
#include <thread>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/File.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"
#include "Game/World/Pipe.hpp"

using namespace tinyxml2;

Level::Level( char const * levelName )
	: m_levelName( levelName )
{
}

Level::~Level()
{
	delete m_renderingPath;
	delete m_levelScene;

	DebugRendererShutdown();

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

void Level::Startup()
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

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, m_camera );

	// Battle Scene
	m_levelScene = new Scene();
	m_levelScene->AddLight( *m_lightSources[0] );
	m_levelScene->AddRenderable( *m_lightSources[0]->m_renderable );
	m_levelScene->AddCamera( *m_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
}

void Level::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Level::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );
}

void Level::Render() const
{
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
	
	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);

	DebugRendererRender();
}