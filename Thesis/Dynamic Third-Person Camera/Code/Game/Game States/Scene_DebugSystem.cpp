#pragma once
#include "Scene_DebugSystem.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CB_FreeLook.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Building.hpp"

Scene_DebugSystem::Scene_DebugSystem( Clock const *parentClock )
	: GameState( "DEBUG SYSTEM", parentClock )
{
	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );
	m_scene			= new Scene();

	// Setting up the Camera
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight->UsesShadowMap( true );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_simple.png", TERRAIN_GRASS );
	AddNewGameObjectToScene( m_terrain, ENTITY_TERRAIN );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );

	// Set the Raycast std::function
	raycast_std_func terrainRaycastStdFunc;
	terrainRaycastStdFunc = [ this ]( Vector3 const &startPos, Vector3 const &direction, float maxDistance ) { return Raycast( startPos, direction, maxDistance ); };
	m_cameraManager->SetRaycastCallback( terrainRaycastStdFunc );
	
	// Camera Behaviour
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", *m_cameraManager );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->SetActiveCameraBehaviourTo( "FreeLook" );
}

Scene_DebugSystem::~Scene_DebugSystem()
{
	// Camera Behaviour
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

void Scene_DebugSystem::JustFinishedTransition()
{
	DebugRendererChange3DCamera( m_camera );
}

void Scene_DebugSystem::BeginFrame()
{

}

void Scene_DebugSystem::EndFrame()
{

}

void Scene_DebugSystem::Update()
{
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	m_cameraManager->PreUpdate();

	// Update Debug Renderer Objects
	DebugRendererUpdate( m_clock );

	// Update Game Objects
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
}

void Scene_DebugSystem::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// Render the Scene
	g_theRenderer->SetAmbientLight( m_ambientLight );
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_scene, nullptr );

	// DEBUG
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Debug Renderer
	DebugRendererRender();
}

RaycastResult Scene_DebugSystem::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance )
{

	RaycastResult closestResult( startPosition );

	// Do Terrain Raycast
	closestResult = m_terrain->Raycast( startPosition, direction, maxDistance, 0.05f );

	// Do Building Raycast
	GameObjectList &buildings = m_gameObjects[ ENTITY_BUILDING ];
	for( int i = 0; i < buildings.size(); i++ )
	{
		Building		*thisBuilding		= (Building*)buildings[i];
		RaycastResult	 buildingHitResult	= thisBuilding->Raycast( startPosition, direction, maxDistance, 0.2f );

		// If this one is the closest hit point, from start position
		if( closestResult.didImpact == true )
		{
			if( buildingHitResult.didImpact == true )
			{
				if( buildingHitResult.fractionTravelled < closestResult.fractionTravelled )
					closestResult = buildingHitResult;
			}
		}
		else
		{
			if( buildingHitResult.didImpact == true )
				closestResult = buildingHitResult;
		}
	}

	return closestResult;
}

void Scene_DebugSystem::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_DebugSystem::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}
