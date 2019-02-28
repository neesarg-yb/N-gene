#pragma once
#include "Scene_CollisionAvoidance.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Building.hpp"
#include "Game/World/House.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"
#include "Game/Camera System/Camera Behaviours/CB_ShoulderView.hpp"
#include "Game/Camera System/Camera Constraints/CC_LineOfSight.hpp"
#include "Game/Camera System/Camera Constraints/CC_ConeRaycast.hpp"
#include "Game/Camera System/Camera Constraints/CC_ModifiedConeRaycast.hpp"
#include "Game/Camera System/Camera Constraints/CC_CameraCollision.hpp"

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
	m_debugCamera = new DebugCamera( m_debugFreelook, g_theInput );
	m_debugCamera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, 0.0001f, 1000.f );
	g_activeDebugCamera = m_debugCamera;

	// Add to Scene
	m_scene->AddCamera( *m_debugCamera );

	// A directional light
	Light *directionalLight = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight->UsesShadowMap( true );

	// Add to Scene
	AddNewLightToScene( directionalLight );

	// Terrain
	m_terrain = new Terrain( Vector3( -125.f, -25.f, -125.f ), IntVector2( 180, 180 ), 30.f, "Data\\Images\\Terrain\\heightmap_simple.png", TERRAIN_GRASS );
	AddNewGameObjectToScene( m_terrain, ENTITY_TERRAIN );

	// House
	House *testHouse = new House( Vector2::ZERO, 5.f, 5.f, 6.5f, 0.5f, *m_terrain );
	AddNewGameObjectToScene( testHouse, ENTITY_HOUSE );

	// Just a terrace (balcony?)
	Building *tBuilding = new Building( Vector2( 0.f, -30.f), 1.f, 25.f, *m_terrain, 2.5f );
	AddNewGameObjectToScene( tBuilding, ENTITY_BUILDING );

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
	CameraBehaviour* followBehaviour	= new CB_Follow( 6.3f, 80.f, 70.f, 100.f/*, 1.f, 179.f*/, "Follow", m_cameraManager );
	CameraBehaviour* shoulderBehavior	= new CB_ShoulderView( 0.17f, 0.33f, 0.36f, -45.f, 45.f, "Shoulder View", m_cameraManager );
	m_cameraManager->AddNewCameraBehaviour( followBehaviour );
	m_cameraManager->AddNewCameraBehaviour( shoulderBehavior );

	m_proportionalController = new CMC_ProportionalController( "Proportional Controller", m_cameraManager );
//	m_cameraManager->SetActiveMotionControllerTo( m_proportionalController );

	// Camera Constraints
	CC_LineOfSight*			losConstarin		= new CC_LineOfSight( "LineOfSight", *m_cameraManager, 3 );
	CC_ConeRaycast*			conRaycastCC		= new CC_ConeRaycast( "ConeRaycast", *m_cameraManager, 2 );
	CC_ModifiedConeRaycast*	modConRaycastCC		= new CC_ModifiedConeRaycast( "M_ConeRaycast", *m_cameraManager, 1, (CB_Follow *)followBehaviour );
	CC_CameraCollision*	collisionConstrain		= new CC_CameraCollision( "CameraCollision", *m_cameraManager, 4 );
	m_cameraManager->RegisterConstraint( losConstarin );
	m_cameraManager->RegisterConstraint( conRaycastCC );
	m_cameraManager->RegisterConstraint( modConRaycastCC );
	m_cameraManager->RegisterConstraint( collisionConstrain );

	followBehaviour->m_constraints.SetOrRemoveTags( "M_ConeRaycast" );
	TODO( "CameraCollision contraint is not working as expected when I enabled culling to none!" );
	followBehaviour->m_constraints.SetOrRemoveTags( "CameraCollision" );
	followBehaviour->m_constraints.SetOrRemoveTags( "LineOfSight" );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
//	m_cameraManager->SetActiveCameraBehaviourTo( "Follow" );
	m_cameraManager->SetActiveCameraBehaviourTo( "Shoulder View" );
}

Scene_CollisionAvoidance::~Scene_CollisionAvoidance()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "Follow" );
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

	delete m_proportionalController;
	m_proportionalController = nullptr;

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
	Matrix44 inputRefCamMatrix	= m_cameraManager->GetCameraMatrixForInputReference();
	Vector3	 cameraForward		= inputRefCamMatrix.GetKColumn();
	DebugRenderVector( 0.f, m_player->m_transform.GetWorldPosition(), cameraForward, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );
	m_player->InformAboutCameraForward( cameraForward );
	
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

	// Debug Print for visualizing the input interpolation done by camera system
	Matrix44 actualCamMatrix	= m_camera->m_cameraTransform.GetWorldTransformMatrix();
	Vector2  rightStickVisPos	= Vector2( 139.f, -340.f );
	DebugRenderRightStickInputVisualizer( rightStickVisPos, inputRefCamMatrix, actualCamMatrix );
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
			// Since we're not rendering on this camera, we need to update its view matrix manually for DebugRenderCamera()'s frustum
			m_camera->UpdateViewMatrix();

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
	PROFILE_SCOPE_FUNCTION();

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

	// Do House Raycast
	GameObjectList &houses = m_gameObjects[ ENTITY_HOUSE ];
	for( int i = 0; i < houses.size(); i++ )
	{
		House			*thisHouse		= (House*)houses[i];
		RaycastResult	 houseHitResult	= thisHouse->Raycast( startPosition, direction, maxDistance, 0.2f );

		// If this one is the closest hit point, from start position
		if( closestResult.didImpact == true )
		{
			if( houseHitResult.didImpact == true )
			{
				if( houseHitResult.fractionTravelled < closestResult.fractionTravelled )
					closestResult = houseHitResult;
			}
		}
		else
		{
			if( houseHitResult.didImpact == true )
				closestResult = houseHitResult;
		}
	}

	return closestResult;
}

Vector3 Scene_CollisionAvoidance::SphereCollision( Sphere const &sphere )
{
	PROFILE_SCOPE_FUNCTION();

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

void Scene_CollisionAvoidance::DebugRenderRightStickInputVisualizer( Vector2 screenPosition, Matrix44 const &inputReferenceMat, Matrix44 const &actualCameraMat )
{
	float indicatorLength = 100.f;
	Vector2 leftStick = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;

	//---------------------------------------
	// Input relative to the REFERENCE matrix
	Vector3 cameraForward = inputReferenceMat.GetKColumn();
	cameraForward.y = 0.f;
	if( cameraForward.GetLength() != 0.f )
		cameraForward = cameraForward.GetNormalized();
	else
		cameraForward = Vector3::ZERO;

	Vector3	cameraRight				= Vector3::CrossProduct( Vector3::UP, cameraForward );
	Vector3	forceRelativeToCamera	= ( cameraForward * leftStick.y ) + ( cameraRight * leftStick.x );
	Vector2	refMatDebugLineEndPoint	= (Vector2( forceRelativeToCamera.x, forceRelativeToCamera.z ) * indicatorLength) + screenPosition;

	//---------------------------------------
	// Input relative to ACTUAL CAMERA matrix
	cameraForward = actualCameraMat.GetKColumn();
	cameraForward.y = 0.f;
	if( cameraForward.GetLength() != 0.f )
		cameraForward = cameraForward.GetNormalized();
	else
		cameraForward = Vector3::ZERO;

			cameraRight				= Vector3::CrossProduct( Vector3::UP, cameraForward );
			forceRelativeToCamera	= ( cameraForward * leftStick.y ) + ( cameraRight * leftStick.x );
	Vector2 camMatDebugLineEndPoint	= (Vector2( forceRelativeToCamera.x, forceRelativeToCamera.z ) * indicatorLength) + screenPosition;
	

	// Points to draw the frame
	AABB2	frameBounds				= AABB2( screenPosition, indicatorLength * 1.2f, indicatorLength * 1.2f );
	Vector2 topMiddle				= Vector2( (frameBounds.mins.x + frameBounds.maxs.x) * 0.5f, frameBounds.maxs.y );
	Vector2 botMiddle				= Vector2( topMiddle.x, frameBounds.mins.y );
	Vector2 rightMiddle				= Vector2( frameBounds.maxs.x, (frameBounds.mins.y + frameBounds.maxs.y) * 0.5f );
	Vector2 leftMiddle				= Vector2( frameBounds.mins.x, rightMiddle.y );

	// Frame
	DebugRender2DQuad( 0.f, frameBounds, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR );
	
	// Boundary Circle
	DebugRender2DRound( 0.f, screenPosition, indicatorLength * 1.05f, 25U, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );
	DebugRender2DRound( 0.f, screenPosition, indicatorLength * 1.03f, 25U, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR );
	
	// Cross with lines
	DebugRender2DLine( 0.f, botMiddle, RGBA_GRAY_COLOR, topMiddle, RGBA_GRAY_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
	DebugRender2DLine( 0.f, leftMiddle, RGBA_GRAY_COLOR, rightMiddle, RGBA_GRAY_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );

	// World direction labels: North, West, East, South
	DebugRender2DText( 0.f, Vector2(topMiddle.x + 2.f, topMiddle.y - 16.f), 15.f, RGBA_RED_COLOR, RGBA_RED_COLOR, "N" );
	DebugRender2DText( 0.f, Vector2(botMiddle.x + 1.f, botMiddle.y - 1.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, "S" );
	DebugRender2DText( 0.f, Vector2(leftMiddle.x, leftMiddle.y), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, "W" );
	DebugRender2DText( 0.f, Vector2(rightMiddle.x - 15.f, rightMiddle.y), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, "E" );

	// Interpolated Player input relative to work
	DebugRender2DLine( 0.f, screenPosition, RGBA_GRAY_COLOR, camMatDebugLineEndPoint, RGBA_GRAY_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );		// Actual Camera Matrix - Input Line
	DebugRender2DLine( 0.f, screenPosition, RGBA_KHAKI_COLOR, refMatDebugLineEndPoint, RGBA_RED_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );		// Reference Matrix - Input Line
}
