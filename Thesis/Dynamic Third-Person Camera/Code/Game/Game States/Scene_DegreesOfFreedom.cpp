#pragma once
#include "Scene_DegreesOfFreedom.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CB_DegreesOfFreedom.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Terrain.hpp"

Scene_DegreesOfFreedom::Scene_DegreesOfFreedom()
	: GameState( "DEGREES OF FREEDOM" )
{
	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
	m_scene			= new Scene();

	// Setting up the Camera
	m_camera = new OrbitCamera( Vector3::ZERO );
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_currentFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
//	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_rivers.png" );
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_simple.png" );
	AddNewGameObjectToScene( m_terrain );

	// Player
	Vector3 inFrontOfCamera  = m_camera->m_cameraTransform.GetWorldPosition();
	inFrontOfCamera.y		-= 0.f;
	inFrontOfCamera.z		+= 5.f;
	m_player = new Player( inFrontOfCamera, *m_terrain );
	AddNewGameObjectToScene( m_player );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput );
	m_cameraManager->SetAnchor( m_player );

	// Degrees of Freedom - Camera Behavior
	CameraBehaviour* dofBehaviour = new CB_DegreesOfFreedom( 5.f, 20.f, 30.f, 100.f, "DegreesOfFreedom" );
	m_cameraManager->AddNewCameraBehaviour( dofBehaviour );
	m_cameraManager->SetActiveCameraBehaviourTo( "DegreesOfFreedom" );
}

Scene_DegreesOfFreedom::~Scene_DegreesOfFreedom()
{
	m_cameraManager->DeleteCameraBehaviour( "DegreesOfFreedom" );

	// Camera Manager
	delete m_cameraManager;
	m_cameraManager = nullptr;

	// Player
	m_player = nullptr;		// Gets deleted from m_gameObjects

	// Terrain
	m_terrain = nullptr;	// Gets deleted from m_gameObjects

	// GameObjects
	for each (GameObject* go in m_gameObjects)
		go->RemoveRenderablesFromScene( *m_scene );

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
}

void Scene_DegreesOfFreedom::BeginFrame()
{
	m_cameraManager->PostUpdate();
}

void Scene_DegreesOfFreedom::EndFrame()
{
	m_cameraManager->PostUpdate();
}

void Scene_DegreesOfFreedom::Update( float deltaSeconds )
{
	m_player->InformAboutCameraForward( m_camera->GetForwardVector() );

	// Update Game Objects
	for each (GameObject* go in m_gameObjects)
		go->Update( deltaSeconds );

	// Update Camera Stuffs
	UpdateCameraFOV( deltaSeconds );
	m_cameraManager->Update( deltaSeconds );

	// Update Debug Renderer Objects
	DebugRendererUpdate( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_DegreesOfFreedom::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	
	// Render the Scene
	g_theRenderer->SetAmbientLight( m_ambientLight );
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_scene );

	// Debug Renderer
	TODO( "DebugRenderer getting started from Scene_QuaternionTets.. This is a hot fix for now." );
	DebugRendererChange3DCamera( m_camera );
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	DebugRendererRender();
}

void Scene_DegreesOfFreedom::AddNewGameObjectToScene( GameObject *go )
{
	// Add game object which gets updated every frame
	m_gameObjects.push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_DegreesOfFreedom::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

void Scene_DegreesOfFreedom::UpdateCameraFOV( float deltaSeconds )
{
	XboxController &controller = g_theInput->m_controller[0];

	// LT & RT - Zoom out & Zoom in
	float leftTrigger	= controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];
	float rightTrigger	= controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];

	m_currentFOV += ( leftTrigger - rightTrigger ) * m_changeFOVSpeed * deltaSeconds;
	m_camera->SetPerspectiveCameraProjectionMatrix( m_currentFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
}
