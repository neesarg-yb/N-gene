#pragma once
#include "Scene_ProtoScene3D.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"

Scene_ProtoScene3D::Scene_ProtoScene3D( Clock const *parentClock )
	: GameState( "PROTOSCENE 3D", parentClock )
{
	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
	m_scene			= new Scene();

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight->UsesShadowMap( true );
	AddNewLightToScene( directionalLight );
	
	// Setting up the Camera
	m_camera = new MCamera( *g_theRenderer );
	m_scene->AddCamera( *m_camera->GetCamera() );

	// Loading Models
	Vector3 snowMikuPosition = Vector3( -5.f, -3.f, 20.f );						// SNOW MIKU
	Vector3 snowMikuRotation = Vector3( 0.f, 180.f, 0.f );
	m_snowMiku		= new Renderable( snowMikuPosition, snowMikuRotation, Vector3::ONE_ALL );
	bool mikuLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\snow_miku\\ROOMITEMS011_ALL.obj", *m_snowMiku );
	GUARANTEE_RECOVERABLE( mikuLoaded, "Snow Miku obj model loading FAILED!" );
	
	Vector3 spaceshipPosition = Vector3( 5.f, -3.f, 21.f );
	Vector3 spaceshipRotation = Vector3( 0.f, 180.f, 0.f );
	m_spaceship		= new Renderable( spaceshipPosition, spaceshipRotation, Vector3::ONE_ALL );
	bool shipLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\scifi_fighter_mk6\\scifi_fighter_mk6.obj", *m_spaceship );
	GUARANTEE_RECOVERABLE( shipLoaded, "Spaceship obj model loading FAILED" );

	if( mikuLoaded )
		m_scene->AddRenderable( *m_snowMiku );
	if( shipLoaded )
		m_scene->AddRenderable( *m_spaceship );
}

Scene_ProtoScene3D::~Scene_ProtoScene3D()
{
	// GameObjects
	for( int i = 0; i < NUM_ENTITIES; i++ )
	{
		while ( m_gameObjects[i].size() > 0 )
		{
			GameObject* &lastGameObject = m_gameObjects[i].back();

			lastGameObject->RemoveRenderablesFromScene( *m_scene );
			delete lastGameObject;
			lastGameObject = nullptr;

			m_gameObjects[i].pop_back();
		}
	}

	// Lights
	while ( m_lights.size() > 0 )
	{
		// Get the light from the back of m_lights
		Light* lastLight = m_lights.back();
		m_lights.pop_back();

		// Remove its renderable
		m_scene->RemoveRenderable( *lastLight->m_renderable );

		// Delete the light
		delete lastLight;
		lastLight = nullptr;
	}

	// Camera
	delete m_camera;
	m_camera = nullptr;

	// Scene
	delete m_scene;
	m_scene = nullptr;

	// Forward Rendering Path
	delete m_renderingPath;
	m_renderingPath = nullptr;

	// Obj Models
	delete m_snowMiku;
	m_snowMiku = nullptr;

	delete m_spaceship;
	m_spaceship = nullptr;
}

void Scene_ProtoScene3D::JustFinishedTransition()
{

}

void Scene_ProtoScene3D::BeginFrame()
{
	PROFILE_SCOPE_FUNCTION();

	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );

	DebugRender2DText( 0.f, Vector2( -850.f, 460.f ), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, "Mouse & WASD-QE: to move around." );
}

void Scene_ProtoScene3D::EndFrame()
{

}

void Scene_ProtoScene3D::Update()
{
	PROFILE_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();
	ProcessInput( deltaSeconds );

	// Update Game Objects
	for( int i = 0; i < NUM_ENTITIES; i++ )
	{
		for each( GameObject* go in m_gameObjects[i] )
			go->Update( deltaSeconds );
	}

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );

	// Debug Render
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	m_camera->RebuildMatrices();
}

void Scene_ProtoScene3D::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	m_renderingPath->RenderScene( *m_scene );
}

void Scene_ProtoScene3D::ProcessInput( float deltaSeconds )
{
	// Camera Rotation
	Vector3 camRotation = m_camera->GetRotation();
	Vector2 mouseChange = g_theInput->GetMouseDelta();

	camRotation.y += mouseChange.x * m_camRotationSpeed * deltaSeconds;
	camRotation.x += mouseChange.y * m_camRotationSpeed * deltaSeconds;

	m_camera->SetRotation( camRotation );

	// Camera Position
	Vector3 camPosition		= m_camera->GetPosition();
	Vector3 positionChange	= Vector3::ZERO;

	if( g_theInput->IsKeyPressed( 'W' ) )
		positionChange.z += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'S' ) )
		positionChange.z -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'D' ) )
		positionChange.x += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'A' ) )
		positionChange.x -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		positionChange.y += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'E' ) )
		positionChange.y -= m_flySpeed * deltaSeconds;

	// Position change relative to camera's new rotation
	Vector2 xDir = Vector2( CosDegree(camRotation.y), SinDegree(camRotation.y) );
	Vector2 zDir = Vector2( -xDir.y, xDir.x );

	positionChange.x = Vector2::DotProduct( xDir, Vector2(positionChange.x, positionChange.z) );
	positionChange.z = Vector2::DotProduct( zDir, Vector2(positionChange.x, positionChange.z) );

	m_camera->SetPosition( camPosition + positionChange );
}

void Scene_ProtoScene3D::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_ProtoScene3D::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}
