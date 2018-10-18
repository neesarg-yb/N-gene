#pragma once
#include "Scene_DegreesOfFreedom.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Potential Engine/CB_Follow.hpp"
#include "Game/Potential Engine/CB_FreeLook.hpp"
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
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_simple.png" );
	AddNewGameObjectToScene( m_terrain );

	// Player
	Vector3 inFrontOfCamera  = m_camera->m_cameraTransform.GetWorldPosition();
	inFrontOfCamera.y		-= 0.f;
	inFrontOfCamera.z		+= 5.f;
	m_player = new Player( inFrontOfCamera, *m_terrain );
	AddNewGameObjectToScene( m_player );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, m_cameraNear );
	m_cameraManager->SetAnchor( m_player );

	// Degrees of Freedom - Camera Behavior
	CameraBehaviour* dofBehaviour		= new CB_Follow( 5.f, 40.f, 30.f, 100.f, "Follow" );
	CameraBehaviour* followBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook" );
	m_cameraManager->AddNewCameraBehaviour( dofBehaviour );
	m_cameraManager->SetActiveCameraBehaviourTo( "Follow" );
	m_cameraManager->AddNewCameraBehaviour( followBehaviour );
}

Scene_DegreesOfFreedom::~Scene_DegreesOfFreedom()
{
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );
	m_cameraManager->DeleteCameraBehaviour( "Follow" );

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

void Scene_DegreesOfFreedom::JustFinishedTransition()
{
	DebugRendererChange3DCamera( m_camera );
}

void Scene_DegreesOfFreedom::BeginFrame()
{
	ChangeCameraBehaviour();

	m_cameraManager->PreUpdate();
}

void Scene_DegreesOfFreedom::EndFrame()
{
	m_cameraManager->PostUpdate();
}

void Scene_DegreesOfFreedom::Update( float deltaSeconds )
{
	// Update Debug Renderer Objects
	DebugRendererUpdate( deltaSeconds );

	m_player->InformAboutCameraForward( m_camera->GetForwardVector() );

	// Update Game Objects
	for each (GameObject* go in m_gameObjects)
		go->Update( deltaSeconds );

	// Update Camera Stuffs
	m_cameraManager->Update( deltaSeconds );

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

	// DEBUG
	Matrix44 bMat;
	bMat.SetTColumn( Vector3( 0.f, 0.f, 1.f ) );

	DebugRenderBasis( 0.f, Matrix44(),	RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	DebugRenderBasis( 0.f, bMat,		RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );
	
	// Debug Renderer
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

void Scene_DegreesOfFreedom::ChangeCameraBehaviour()
{
	XboxController &controller	= g_theInput->m_controller[0];
	bool leftStickJustPressed	= controller.m_xboxButtonStates[ XBOX_BUTTON_LS ].keyJustPressed;

	static bool dofBehaviourActive = true;
	if( leftStickJustPressed )
	{
		std::string behaviourToActivate = dofBehaviourActive ? "FreeLook" : "Follow";
		dofBehaviourActive = !dofBehaviourActive;

		m_cameraManager->SetActiveCameraBehaviourTo( behaviourToActivate );

		std::string activeBehaviourMessage = Stringf( "Active Camera Behavior: \"%s\"", behaviourToActivate.c_str() );
		DebugRender2DText( 5.f, Vector2(-850.f, 460.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, activeBehaviourMessage.c_str() );
	}

}
