#pragma once
#include "Scene_EfficientRaycast.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Building.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"

Scene_EffecientRaycast::Scene_EffecientRaycast( Clock const *parentClock )
	: GameState( "EFFICIENT RAYCASTS", parentClock )
{
	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
	m_scene			= new Scene();

	// Setting up the Camera
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	m_camera->EnableShadowMap();
	m_camera->RenderDebugObjects( true );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight->UsesShadowMap( true );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 180, 180 ), 30.f, "Data\\Images\\Terrain\\heightmap_simple.png", TERRAIN_GRASS );
	AddNewGameObjectToScene( m_terrain, ENTITY_TERRAIN );

	// Buildings
	for( int i = 0; i < m_maxBuildings; i++ )
	{
		Vector2 worldMins	= Vector2( m_terrain->m_worldBounds.mins.x, m_terrain->m_worldBounds.mins.z );
		Vector2 worldMaxs	= Vector2( m_terrain->m_worldBounds.maxs.x, m_terrain->m_worldBounds.maxs.z );
		Vector2 positionXZ	= Vector2( 2.f, 10.f );
		
		// positionXZ.x = GetRandomFloatInRange( worldMins.x, worldMaxs.x );
		// positionXZ.y = GetRandomFloatInRange( worldMins.y, worldMaxs.y );

		Building *aBuilding = new Building( positionXZ, 25.f, 5.f, *m_terrain );
		AddNewGameObjectToScene( aBuilding, ENTITY_BUILDING );
	}

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );

	// Set the Raycast std::function
	raycast_std_func terrainRaycastStdFunc;
	terrainRaycastStdFunc = [ this ]( Vector3 const &startPos, Vector3 const &direction, float maxDistance ) { return Raycast( startPos, direction, maxDistance ); };
	m_cameraManager->SetRaycastCallback( terrainRaycastStdFunc );

	// Camera Behaviour
	CameraBehaviour *freelookBehavior = new CB_FreeLook( 15.f, 90.f, -50.f, 50.f, "Freelook", m_cameraManager, USE_KEYBOARD_MOUSE_FL );
	m_cameraManager->AddNewCameraBehaviour( freelookBehavior );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->ChangeCameraBehaviourTo( "Freelook", 0.f );
}

Scene_EffecientRaycast::~Scene_EffecientRaycast()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "Follow" );
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

	// Camera Manager
	delete m_cameraManager;
	m_cameraManager = nullptr;

	// Terrain
	m_terrain = nullptr;	// Gets deleted from m_gameObjects

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

void Scene_EffecientRaycast::JustFinishedTransition()
{
	g_theInput->ShowCursor( false );
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
}

void Scene_EffecientRaycast::BeginFrame()
{
	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );
}

void Scene_EffecientRaycast::EndFrame()
{

}

void Scene_EffecientRaycast::Update()
{
	m_cameraManager->PreUpdate();

	// All DebugRender must be added during update (not during render)
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Update Game Objects
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();
	for( int i = 0; i < NUM_ENTITIES; i++ )
	{
		for each( GameObject* go in m_gameObjects[i] )
			go->Update( deltaSeconds );
	}

	// Update Camera Stuffs
	m_cameraManager->Update( deltaSeconds );

	// Test Raycast
	CameraState	currCameraState			= m_cameraManager->GetCurrentCameraState();
	Matrix44	cameraTransformMatrix	= currCameraState.GetTransformMatrix();
	PerformDebugTestRaycasts( cameraTransformMatrix );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
	{
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		g_theInput->ShowCursor( true );

		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	}

	m_cameraManager->PostUpdate();
}

void Scene_EffecientRaycast::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	
	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	m_renderingPath->RenderScene( *m_scene );

	DebugRendererLateRender( m_camera );
}

RaycastResult Scene_EffecientRaycast::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const
{
	Building const *firstBuilding = (Building const *) m_gameObjects[ ENTITY_BUILDING ][0];

	return firstBuilding->DoPerfectRaycast( startPosition, direction, maxDistance );
}

void Scene_EffecientRaycast::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_EffecientRaycast::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

void Scene_EffecientRaycast::PerformDebugTestRaycasts( Matrix44 const &cameraTransformMat )
{
	Vector3 rayStartPos	 = cameraTransformMat.GetTColumn();
	Vector3 rayDirection = cameraTransformMat.GetKColumn();

	// Do the raycast
	if( g_theInput->WasKeyJustPressed( 'R' ) )
	{
		m_rayStartPos	= rayStartPos;
		m_raycastResult	= Raycast( rayStartPos, rayDirection, m_rayMaxLength );
	}

	// Debug Render
	DebugRenderRaycast( 0.f, m_rayStartPos, m_raycastResult, 0.5f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_PURPLE_COLOR, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY ); 
}
