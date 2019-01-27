#pragma once
#include "Scene_ProtoScene3D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/DevConsole.hpp"
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
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	m_camera->EnableShadowMap();
	m_camera->RenderDebugObjects( true );
	m_scene->AddCamera( *m_camera );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );
	m_cameraManager->SetAnchor( nullptr );

	// Camera Behaviour
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", m_cameraManager, USE_KEYBOARD_MOUSE_FL );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );
	m_cameraManager->SetActiveCameraBehaviourTo( "FreeLook" );					// MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR
}

Scene_ProtoScene3D::~Scene_ProtoScene3D()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

	// Camera Manager
	delete m_cameraManager;
	m_cameraManager = nullptr;

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
}

void Scene_ProtoScene3D::JustFinishedTransition()
{

}

void Scene_ProtoScene3D::BeginFrame()
{
	PROFILE_SCOPE_FUNCTION();

	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );
}

void Scene_ProtoScene3D::EndFrame()
{

}

void Scene_ProtoScene3D::Update()
{
	PROFILE_SCOPE_FUNCTION();
	
	m_cameraManager->PreUpdate();

	// Update Game Objects
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();
	for( int i = 0; i < NUM_ENTITIES; i++ )
	{
		for each( GameObject* go in m_gameObjects[i] )
			go->Update( deltaSeconds );
	}

	// Update Camera Stuffs
	m_cameraManager->Update( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );

	m_cameraManager->PostUpdate();

	// Debug Render
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );
}

void Scene_ProtoScene3D::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	m_renderingPath->RenderScene( *m_scene );
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
