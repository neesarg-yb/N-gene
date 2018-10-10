#pragma once
#include "Scene_FollowCamera.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CC_LineOfSight.hpp"
#include "Game/Potential Engine/CB_FreeLook.hpp"
#include "Game/Potential Engine/CB_Follow.hpp"
#include "Game/theGame.hpp"

Scene_FollowCamera::Scene_FollowCamera()
	: GameState( "FOLLOW CAMERA" )
{
	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
	m_scene			= new Scene();

	// Setting up the Camera
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\galaxy1.png" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_rivers.png" );
	AddNewGameObjectToScene( m_terrain );

	// Player
	m_player = new Player( Vector3( 0.f, 0.f, 15.f ), *m_terrain );
	AddNewGameObjectToScene( m_player );
	
	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput );
	m_cameraManager->SetAnchor( m_player );

	// Set the Raycast std::function
	raycast_std_func terrainRaycastStdFunc;
	terrainRaycastStdFunc = [ this ]( Vector3 const &startPos, Vector3 const &direction, float maxDistance ) { return m_terrain->Raycast( startPos, direction, maxDistance, 0.05f ); };
	m_cameraManager->SetRaycastCallback( terrainRaycastStdFunc );

	// Camera Behaviour
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook" );
	CameraBehaviour* followBehaviour	= new CB_Follow( 5.f, 40.f, 30.f, 100.f, "Follow" );
	m_cameraManager->AddNewCameraBehaviour( followBehaviour );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );

	// Camera Constrains
	CC_LineOfSight* losConstarin = new CC_LineOfSight( "LineOfSight", *m_cameraManager, 2 );
	m_cameraManager->RegisterConstrain( losConstarin );
	followBehaviour->m_constrains.SetOrRemoveTags( "LineOfSight" );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->SetActiveCameraBehaviourTo( "Follow" );
}

Scene_FollowCamera::~Scene_FollowCamera()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

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

void Scene_FollowCamera::BeginFrame()
{
	ChangeCameraBehaviour();

	m_cameraManager->PreUpdate();
}

void Scene_FollowCamera::EndFrame()
{
	m_cameraManager->PostUpdate();
}

void Scene_FollowCamera::Update( float deltaSeconds )
{
	m_player->InformAboutCameraForward( m_camera->GetForwardVector() );

	// Update Game Objects
	for each (GameObject* go in m_gameObjects)
		go->Update( deltaSeconds );

	// Update Camera Stuffs
	EnableDisableCameraConstrains();
	m_cameraManager->Update( deltaSeconds );

	// Update Debug Renderer Objects
	DebugRendererUpdate( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_FollowCamera::Render( Camera *gameCamera ) const
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

void Scene_FollowCamera::AddNewGameObjectToScene( GameObject *go )
{
	// Add game object which gets updated every frame
	m_gameObjects.push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_FollowCamera::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

void Scene_FollowCamera::ChangeCameraBehaviour()
{
	return;
}

void Scene_FollowCamera::EnableDisableCameraConstrains()
{
	XboxController &controller = g_theInput->m_controller[0];
	bool toggleConstrainButton = controller.m_xboxButtonStates[ XBOX_BUTTON_X ].keyJustPressed;

	if( toggleConstrainButton == true )
	{
		// Toggle the constrain
		m_constrainsActive = !m_constrainsActive;
		m_cameraManager->EnableConstrains( m_constrainsActive );

		// Debug Print
		float		yOffset;
		Rgba		constarinsColor;
		std::string constrainsActiveStr = Stringf( "Camera Constrains: ", m_constrainsActive );
		if( m_constrainsActive )
		{
			constrainsActiveStr += "ENABLED";
			constarinsColor		 = RGBA_PURPLE_COLOR;
			yOffset				 = 0.f;
		}
		else
		{
			constrainsActiveStr += "DISABLED";
			constarinsColor		 = RGBA_RED_COLOR;
			yOffset				 = 15.f;
		}

		DebugRender2DText( 3.f, Vector2(-850.f, 460.f - yOffset), 15.f, constarinsColor, constarinsColor, constrainsActiveStr.c_str() );
	}
}

