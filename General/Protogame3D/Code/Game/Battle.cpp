#pragma once
#include "Battle.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"

using namespace tinyxml2;

Scene*				  Battle::s_testScene;
Camera*				  Battle::s_camera;
std::vector< Light* > Battle::s_lightSources;

void Battle::AddNewPointLightToCamareaPosition( Rgba lightColor )
{
	Vector3 cameraPos = s_camera->m_cameraTransform.GetPosition();
	
	Light* newLight	= new Light( cameraPos, s_camera->m_cameraTransform.GetRotation() );
	newLight->SetUpForSpotLight( 40.f, 30.f, 40.f, Vector3( 0.f, 0.f, 1.f ), lightColor );
	s_lightSources.push_back( newLight );

	s_testScene->AddLight( *newLight );
	s_testScene->AddRenderable( *newLight->m_renderable );

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

Battle::Battle()
{
	// Setup the camera
	s_camera = new Camera();
	s_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	s_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() ); 
	s_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 150.f );
	s_camera->LookAt( Vector3( 0.f, 6.f, 0.f ), Vector3( 0.f, 0.f, 20.f ) );
	s_camera->SetupForSkybox( "Data\\Images\\Skybox\\galaxy2.png" );

	// Setup the Lighting
	s_lightSources.push_back( new Light( Vector3::ZERO ) );
	s_lightSources[0]->SetUpForPointLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	s_lightSources[0]->EnablePerimeterRotationAround( Vector3( 0.f, 0.f, 5.f ), 24.f );

	s_lightSources.push_back( new Light( Vector3(0.f, 0.f, 1.f), Vector3( 0.3f, 0.3f, 0.3f) ) );
	s_lightSources[1]->SetUpForSpotLight( 90.f, 20.f, 30.f );

	s_lightSources.push_back( new Light( Vector3( 20.f, 4.f, 10.f ), Vector3( 10.f, -90.f, 0.f) ) );
	s_lightSources[2]->SetUpForDirectionalLight( 30.f, Vector3(1.f, 0.f, 0.f), RGBA_GREEN_COLOR );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, s_camera );
	
	// Setting up the transforms
	m_spaceShipTransform = new Transform();
	m_spaceShipTransform->SetScale( Vector3( 1.f, 1.f, 1.f ) );
	m_spaceShipTransform->SetPosition( Vector3( 0.f, 0.f, 0.f ) );
	
	m_snowMikuTransform	= new Transform();
	m_snowMikuTransform->SetScale( Vector3( 1.f, 1.f, 1.f ) );
	m_snowMikuTransform->SetPosition( Vector3( -7.f, 0.f, 20.f ) );
	m_snowMikuTransform->SetRotation( Vector3( 0.f, 180.f, 0.f ) );
	
	// SpaceShip Obj Model Loading
	m_spaceShip				= new Renderable( *m_spaceShipTransform );
	bool spaceShipLoaded	= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\scifi_fighter_mk6\\scifi_fighter_mk6.obj", *m_spaceShip );
	GUARANTEE_RECOVERABLE( spaceShipLoaded, "Warning: SpaceShip loading from obj file, failed!" );
	m_spaceShipXRay			= new Renderable( Vector3( 0.f, 0.f, 40.f), Vector3( 0.f, 180.f, 0.f), Vector3( 1.f, 1.f, 1.f ) );
	ModelLoader::LoadObjectModelFromPath( "Data\\Models\\scifi_fighter_mk6\\scifi_fighter_mk6.obj", *m_spaceShipXRay );

	m_snowMiku				= new Renderable( *m_snowMikuTransform );
	bool snowMikuLoaded		= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\snow_miku\\ROOMITEMS011_ALL.obj", *m_snowMiku );
	GUARANTEE_RECOVERABLE( snowMikuLoaded, "Warning: SnowMiku loading from obj file, failed!" );
	
	m_testSphereMesh	= MeshBuilder::CreateSphere( 4.f, 30, 30 );
	m_testCubeMesh		= MeshBuilder::CreateCube( Vector3( 4.f, 4.f, 4.f ) );

	// Test Material
	m_cubeMaterial			= Material::CreateNewFromFile( "Data\\Materials\\couch_cube.material" );
	m_sphereMaterial		= Material::CreateNewFromFile( "Data\\Materials\\stone_sphere.material" );
	m_spaceShipXRayMaterial	= Material::CreateNewFromFile( "Data\\Materials\\ship_xray.material" );
	m_spaceShipXRay->SetBaseMaterial( m_spaceShipXRayMaterial );

	// Test Renderables
	m_cube				= new Renderable( Vector3( 0.f, 0.f, 20.f ) );
	m_sphere			= new Renderable( Vector3( 10.f, 3.2f, 20.f) );

	m_cube->SetBaseMesh( m_testCubeMesh );
	m_sphere->SetBaseMesh( m_testSphereMesh );

	m_cube->SetBaseMaterial( m_cubeMaterial );
	m_sphere->SetBaseMaterial( m_sphereMaterial );

	// Test Scene
	s_testScene = new Scene();

	s_testScene->AddRenderable( *m_cube );
	s_testScene->AddRenderable( *m_sphere );
	s_testScene->AddRenderable( *m_spaceShip );
	s_testScene->AddRenderable( *m_snowMiku );
	s_testScene->AddRenderable( *m_spaceShipXRay );

	s_testScene->AddLight( *s_lightSources[0] );
	s_testScene->AddLight( *s_lightSources[1] );
	s_testScene->AddLight( *s_lightSources[2] );

	s_testScene->AddRenderable( *s_lightSources[0]->m_renderable );
	s_testScene->AddRenderable( *s_lightSources[1]->m_renderable );
	s_testScene->AddRenderable( *s_lightSources[2]->m_renderable );

	s_testScene->AddCamera( *s_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
}

Battle::~Battle()
{
	delete m_renderingPath;
	delete s_testScene;

	delete m_spaceShipXRay;
	delete m_spaceShip;
	delete m_sphere;
	delete m_cube;

	delete m_spaceShipXRayMaterial;
	delete m_snowMiku;
	delete m_sphereMaterial;
	delete m_cubeMaterial;
	
	delete m_testCubeMesh;
	delete m_testSphereMesh;
	delete m_snowMikuTransform;
	delete m_spaceShipTransform;
	delete m_spaceShipMesh;
	
	DebugRendererShutdown();

	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		delete s_lightSources[i];
	
	delete s_camera;
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
	
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		s_lightSources[i]->Update( deltaSeconds );
	
	MoveTheCameraAccordingToPlayerInput( deltaSeconds );
	RotateTheCameraAccordingToPlayerInput( deltaSeconds );
	s_lightSources[1]->SetPosition( s_camera->m_cameraTransform.GetPosition() );
	s_lightSources[1]->SetEulerRotation( s_camera->m_cameraTransform.GetRotation() );

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
	m_renderingPath->RenderSceneForCamera( *s_camera, *s_testScene );
	
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

void Battle::MoveTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const movementSpeed = 5.f;		// Units per seconds

	XboxController &inputController = g_theInput->m_controller[0];
	Vector2 axisChnage				= inputController.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	float	leftShoulder			= inputController.m_xboxButtonStates[ XBOX_BUTTON_LB ].keyIsDown ?  1.f : 0.f;
	float	rightShoulder			= inputController.m_xboxButtonStates[ XBOX_BUTTON_RB ].keyIsDown ? -1.f : 0.f;
	float	finalYMovement			= (leftShoulder + rightShoulder) * movementSpeed * deltaSeconds;
	Vector2 finalXZMovement			= axisChnage * movementSpeed * deltaSeconds;

	s_camera->MoveCameraPositionBy( Vector3( finalXZMovement.x, finalYMovement, finalXZMovement.y ) );
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