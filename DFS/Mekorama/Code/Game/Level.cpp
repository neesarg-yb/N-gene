#pragma once
#include "Level.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"
#include "Game/World/Tower.hpp"

using namespace tinyxml2;

Scene*					Level::s_levelScene;
OrbitCamera*			Level::s_camera;
std::vector< Light* >	Level::s_lightSources;

void Level::AddNewPointLightToCamareaPosition( Rgba lightColor )
{
	Vector3 cameraPos = s_camera->m_cameraTransform.GetWorldPosition();
	
	Light* newLight	= new Light( cameraPos, s_camera->m_cameraTransform.GetRotation() );
	newLight->SetUpForPointLight( 40.f, Vector3( 0.f, 0.f, 1.f ), lightColor );
	s_lightSources.push_back( newLight );

	s_levelScene->AddLight( *newLight );
	s_levelScene->AddRenderable( *newLight->m_renderable );

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

Level::Level()
{
	
}

Level::~Level()
{
	BlockDefinition::DeleteAllDefinitions();

	delete m_renderingPath;
	delete s_levelScene;

	delete m_sphere;
	delete m_sphereMaterial;
	delete m_sphereMesh;
	
	DebugRendererShutdown();

	// Lights
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		delete s_lightSources[i];

	// GameObject Pool
	for( unsigned int i = 0; i < m_allGameObjects.size(); i++ )
		delete m_allGameObjects[i];

	m_allGameObjects.clear();
	
	delete s_camera;
}

void Level::Startup()
{
	// Setup the camera
	s_camera = new OrbitCamera( Vector3::ZERO );
	// Set Color Targets
	s_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	s_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	// Projection Matrix
	s_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f ); 
	// Orbit Camera
	s_camera->SetSphericalCoordinate( 10.f, 0.f, 90.f );
	// Skybox
	s_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	s_lightSources.push_back( new Light( Vector3::ZERO ) );
	s_lightSources[0]->SetUpForPointLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	s_lightSources[0]->EnablePerimeterRotationAround( Vector3( 0.f, 0.f, 5.f ), 10.f );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, s_camera );

	// Placeholder sphere for Tank
	m_sphereMesh		= MeshBuilder::CreateSphere( 1.f, 30, 30 );
	m_sphereMaterial	= Material::CreateNewFromFile( "Data\\Materials\\stone_sphere.material" );
	m_sphere			= new Renderable( Vector3( 10.f, 3.2f, 20.f) );

	m_sphere->SetBaseMesh( m_sphereMesh );
	m_sphere->SetBaseMaterial( m_sphereMaterial );

	// Battle Scene
	s_levelScene = new Scene();

	s_levelScene->AddLight( *s_lightSources[0] );

	s_levelScene->AddRenderable( *m_sphere );
	s_levelScene->AddRenderable( *s_lightSources[0]->m_renderable );

	s_levelScene->AddCamera( *s_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );


	// TEST: Definition Loading
	BlockDefinition::LoadAllDefinitions( "Data\\Definitions\\Blocks.xml" );
	TowerDefinition::LoadDefinition( "Data\\Definitions\\Tower1.xml" );

	Tower *testTower = new Tower( Vector3::ZERO, "Tower1" );
	for( uint i = 0; i < testTower->m_allBlocks.size(); i++ )
		s_levelScene->AddRenderable( *testTower->m_allBlocks[i]->m_renderable );
}

void Level::BeginFrame()
{

}

void Level::EndFrame()
{

}

void Level::Update( float deltaSeconds )
{
	// Battle::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Camera Movement
	RotateTheCameraAccordingToPlayerInput( deltaSeconds );DebugRenderPoint( 25.f, 2.f, Vector3::ONE_ALL ,RGBA_RED_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	DebugRenderPoint( 0.f, 2.f, Vector3::ZERO ,RGBA_BLUE_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	
	// Lights
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		s_lightSources[i]->Update( deltaSeconds );

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

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

	// TESTING WORLD TO SCREEN
	Vector2 mousClientPos = g_theInput->GetMouseClientPosition();
	s_camera->GetWorldPositionFromScreen( mousClientPos );
}

void Level::Render() const
{
	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", s_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *s_camera, *s_levelScene );
	
	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) + " [ UP, DOWN ]" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);
	std::string toSpawnSpotLights	= std::string( "Spawn new SpotLights," );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);
	toSpawnSpotLights	= std::string( "Keys: R(red), G(green), B(blue), W(white)" );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);

	DebugRenderPoint( 0.f, 5.f, Vector3( -50.f,  10.f, -50.f ), RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	DebugRenderPoint( 0.f, 5.f, Vector3( -50.f, -10.f, -50.f ), RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	
	DebugRendererRender();
}

double Level::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Level::RotateTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const rotationSpeed = 45.f;	// Degrees per seconds
	static float const altitudeSpeed = 20.f;

	float turnLeftAxis		= 0.f;
	float altitudeUpAxis	= 0.f;

	// Theta Change
	if( g_theInput->IsKeyPressed( 'A' ) || g_theInput->IsKeyPressed( LEFT ) )
		turnLeftAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'D' ) || g_theInput->IsKeyPressed( RIGHT ) )
		turnLeftAxis -= 1.f;

	// Altitude Change
	if( g_theInput->IsKeyPressed( 'W' ) || g_theInput->IsKeyPressed( UP ) )
		altitudeUpAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'S' ) || g_theInput->IsKeyPressed( DOWN ) )
		altitudeUpAxis -= 1.f;

	float cameraRadius	 = s_camera->m_radius;
	float cameraAltitude = s_camera->m_altitude;
	float cameraRotation = s_camera->m_rotation;
	
	// Set Rotation & Altitude
	cameraRotation		+= deltaSeconds * rotationSpeed * -turnLeftAxis;
	cameraAltitude		+= deltaSeconds * altitudeSpeed * -altitudeUpAxis;
	cameraAltitude		 = ClampFloat( cameraAltitude, 30.f, 30.f + 90.f );

	s_camera->SetSphericalCoordinate( cameraRadius, cameraRotation, cameraAltitude );
}

void Level::ChnageLightAsPerInput(float deltaSeconds)
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