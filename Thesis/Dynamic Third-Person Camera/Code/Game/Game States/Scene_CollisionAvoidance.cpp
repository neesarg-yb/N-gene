#pragma once
#include "Scene_CollisionAvoidance.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CB_Follow.hpp"
#include "Game/Potential Engine/CC_LineOfSight.hpp"
#include "Game/Potential Engine/CC_ConeRaycast.hpp"
#include "Game/Potential Engine/CC_CameraCollision.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Building.hpp"

Scene_CollisionAvoidance::Scene_CollisionAvoidance( Clock const *parentClock )
	: GameState( "COLLISION AVOIDANCE", parentClock )
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
	m_camera->RenderDebugObjects( false );
	// Add to Scene
	m_scene->AddCamera( *m_camera );

	// Setting up the Debug Camera
	m_debugFreelook = new CB_FreeLook( 5.f, 35.f, -60.f, 60.f, "DebugFreeLook", nullptr, USE_KEYBOARD_MOUSE_FL );
	m_debugCamera = new DebugCamera( m_debugFreelook );
	m_debugCamera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, 0.0001f, 1000.f );
	// Add to Scene
	m_scene->AddCamera( *m_debugCamera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight->UsesShadowMap( true );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 250, 250 ), 30.f, "Data\\Images\\Terrain\\heightmap_rivers.png", TERRAIN_GRIDLINES );
	AddNewGameObjectToScene( m_terrain, ENTITY_TERRAIN );

	// Buildings
	for( int i = 0; i < TOTAL_BUILDINGS; i ++ )
	{
		Vector2 worldMins = Vector2( m_terrain->m_worldBounds.mins.x, m_terrain->m_worldBounds.mins.z );
		Vector2 worldMaxs = Vector2( m_terrain->m_worldBounds.maxs.x, m_terrain->m_worldBounds.maxs.z );
		Vector2 positionXZ;
		positionXZ.x = GetRandomFloatInRange( worldMins.x, worldMaxs.x );
		positionXZ.y = GetRandomFloatInRange( worldMins.y, worldMaxs.y );

		Building *aBuilding = new Building( positionXZ, 25.f, 5.f, *m_terrain );
		AddNewGameObjectToScene( aBuilding, ENTITY_BUILDING );
	}

	// Player
	m_player = new Player( Vector3( 0.f, 0.f, 15.f ), *m_terrain );
	AddNewGameObjectToScene( m_player, ENTITY_PLAYER );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );
	m_cameraManager->SetAnchor( m_player );

	// Set the Raycast std::function
	raycast_std_func terrainRaycastStdFunc;
	terrainRaycastStdFunc = [ this ]( Vector3 const &startPos, Vector3 const &direction, float maxDistance ) { return Raycast( startPos, direction, maxDistance ); };
	m_cameraManager->SetRaycastCallback( terrainRaycastStdFunc );

	// Set the Sphere Collision std::function
	sphere_collision_func collisionFunc;
	collisionFunc = [ this ] ( Vector3 const &center, float radius )
	{ 
		Sphere cameraRig( center, radius );
		return SphereCollision( cameraRig ); 
	};
	m_cameraManager->SetSphereCollisionCallback( collisionFunc );

	// Camera Behaviour
	// CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", m_cameraManager );
	CameraBehaviour* followBehaviour	= new CB_Follow( 5.f, 40.f, 30.f, 100.f, "Follow", m_cameraManager );
	m_cameraManager->AddNewCameraBehaviour( followBehaviour );

	// Camera Constrains
	CC_LineOfSight*		losConstarin		= new CC_LineOfSight( "LineOfSight", *m_cameraManager, 2 );
	CC_ConeRaycast*		conRaycastCC		= new CC_ConeRaycast( "ConeRaycast", *m_cameraManager, 1 );
	CC_CameraCollision*	collisionConstrain	= new CC_CameraCollision( "CameraCollision", *m_cameraManager, 3 );
	m_cameraManager->RegisterConstrain( losConstarin );
	m_cameraManager->RegisterConstrain( conRaycastCC );
	m_cameraManager->RegisterConstrain( collisionConstrain );
	followBehaviour->m_constrains.SetOrRemoveTags( "ConeRaycast" );
	// followBehaviour->m_constrains.SetOrRemoveTags( "CameraCollision" );
	// followBehaviour->m_constrains.SetOrRemoveTags( "LineOfSight" );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->SetActiveCameraBehaviourTo( "Follow" );
}

Scene_CollisionAvoidance::~Scene_CollisionAvoidance()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "Follow" );
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

	// Camera Manager
	delete m_cameraManager;
	m_cameraManager = nullptr;

	// Player
	m_player = nullptr;		// Gets deleted from m_gameObjects

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

	// Debug Camera
	delete m_debugCamera;
	m_debugCamera = nullptr;

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

void Scene_CollisionAvoidance::JustFinishedTransition()
{
	g_activeDebugCamera = m_debugCamera;

	ConsolePrintf( RGBA_GREEN_COLOR, "Scene_CollisionAvoidance: Press [T] to toggle the debug camera" );
}

void Scene_CollisionAvoidance::BeginFrame()
{
	PROFILE_SCOPE_FUNCTION();
	ChangeDebugCameraSettings();

	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );

	// Debug Camera
	if( m_debugCameraEnabled == true )
		m_debugCamera->Update();
}

void Scene_CollisionAvoidance::EndFrame()
{
	PROFILE_SCOPE_FUNCTION();
	
	// Manipulate Clock after Scene::Update
	// ChangeClocksTimeScale();	// Uses [UP] & [DOWN] keys, which might conflict with CC_ConeRaycast
	PauseUnpauseClock();
}

void Scene_CollisionAvoidance::Update()
{
	PROFILE_SCOPE_FUNCTION();

	if( m_clock->IsPaused() )
		return;
	
	m_cameraManager->PreUpdate();

	// All DebugRender must be added during update (not during render)
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );
	DebugRenderCamera( 0.f, *m_camera, 1.f, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );
	DebugRenderHotkeys();

	// Player moves relative to the direction of camera
	m_player->InformAboutCameraForward( m_camera->GetForwardVector() );
	
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
}

void Scene_CollisionAvoidance::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	Vector3 playerPosition = m_player->m_transform.GetWorldPosition();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	// Debug Camera Active?
	if( m_debugCameraEnabled )
	{
		// Full-screen overlay?
		if( m_debugCameraFullOverlay )
		{
			// Render on just the Debug Camera
			m_renderingPath->RenderSceneForCamera( *m_debugCamera, *m_scene, nullptr );

			// Render the full overlay
			m_debugCamera->RenderAsFulscreenOverlay();
		}
		else
		{
			// Render on both Camera(s)
			m_renderingPath->RenderSceneForCamera( *m_camera, *m_scene, nullptr );
			m_renderingPath->RenderSceneForCamera( *m_debugCamera, *m_scene, nullptr );

			// Render the mini overlay, too!
			m_debugCamera->RenderAsMiniOverlay();
		}
	}
	else
	{
		// Render just on the main camera
		m_renderingPath->RenderSceneForCamera( *m_camera, *m_scene, nullptr );
	}
}

RaycastResult Scene_CollisionAvoidance::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance )
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

Vector3 Scene_CollisionAvoidance::SphereCollision( Sphere const &sphere )
{
	Vector3 positionAfterCollision = sphere.center;

	for( uint i = 0; i < NUM_ENTITIES; i++ )
	{
		GameObjectList &gameObjects = m_gameObjects[i];

		for( uint idx = 0; idx < gameObjects.size(); idx++ )
		{
			bool didCollide			= false;
			positionAfterCollision	= gameObjects[ idx ]->CheckCollisionWithSphere( positionAfterCollision, sphere.radius, didCollide );
		}
	}

	return positionAfterCollision;
}

void Scene_CollisionAvoidance::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_CollisionAvoidance::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

void Scene_CollisionAvoidance::ChangeClocksTimeScale()
{
	if( g_theInput->IsKeyPressed( UP ) )
		m_clock->SetTimeSclae( m_clock->GetTimeScale() + (1.f * GetMasterClock()->GetFrameDeltaSeconds()) );
	if( g_theInput->IsKeyPressed( DOWN ) )
		m_clock->SetTimeSclae( m_clock->GetTimeScale() - (1.f * GetMasterClock()->GetFrameDeltaSeconds()) );

	float clampedTimeScale = ClampFloat( (float)m_clock->GetTimeScale(), 0.f, 3.f );
	m_clock->SetTimeSclae( clampedTimeScale );
}

void Scene_CollisionAvoidance::PauseUnpauseClock()
{
	if( g_theInput->WasKeyJustPressed( 'P' ) )
		m_clock->IsPaused() ? m_clock->Resume() : m_clock->Pause();
}

void Scene_CollisionAvoidance::ChangeDebugCameraSettings()
{
	// Toggle the Debug Camera
	if( g_theInput->WasKeyJustPressed( 'T' ) )
		m_debugCameraEnabled = !m_debugCameraEnabled;

	if( m_debugCameraEnabled )
	{
		// Change the overlay size
		if( g_theInput->WasKeyJustPressed( 'O' ) )
			m_debugCameraFullOverlay = !m_debugCameraFullOverlay;

		g_theInput->ShowCursor( false );
		g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
	}
	else
	{
		g_theInput->ShowCursor( true );
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
	}
}

void Scene_CollisionAvoidance::DebugRenderHotkeys()
{
	// Pause the Game
	std::string pauseGame = Stringf( "[P] Pause/UnPause the Game" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, pauseGame.c_str() );

	// Debug Overlay
	std::string debugCamera  = Stringf( "[T] Debug Camera: %s", m_debugCameraEnabled ? "Enabled" : "Disabled" );
	DebugRender2DText( 0.f, Vector2(-850.f, 440.f), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, debugCamera.c_str() );
	std::string debugOverlay = Stringf( "[O] Debug Overlay: %s", m_debugCameraFullOverlay ? "Full Screen" : "Picture-in-Picture" );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, debugOverlay.c_str() );

	// Mouse Instruction
	std::string mouseLock = Stringf( "Disable the Debug Camera to release mouse!" );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_RED_COLOR, RGBA_RED_COLOR, mouseLock.c_str() );
}
