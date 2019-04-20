#pragma once
#include "Scene_ProportionalController.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/World/Building.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"
#include "Game/Camera System/Camera Behaviours/CB_ProportionalController.hpp"
#include "Game/Camera System/Camera Constraints/CC_LineOfSight.hpp"
#include "Game/Camera System/Camera Constraints/CC_CameraCollision.hpp"

Scene_ProportionalController::Scene_ProportionalController( Clock const *parentClock )
	: GameState( "PROPORTIONAL CONTROLLER", parentClock )
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
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", m_cameraManager, USE_CONTROLLER_FL );
	CameraBehaviour* followBehaviour	= new CB_Follow( 5.f, 40.f, 30.f, 100.f, "Follow", m_cameraManager );
	CameraBehaviour* pcBehaviour		= new CB_ProportionalController( "PC", m_cameraManager );
	m_cameraManager->AddNewCameraBehaviour( followBehaviour );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );
	m_cameraManager->AddNewCameraBehaviour( pcBehaviour );

	// Camera Constraints
	CC_LineOfSight*		losConstarin		= new CC_LineOfSight( "LineOfSight", *m_cameraManager, 2 );
	CC_CameraCollision*	collisionConstrain	= new CC_CameraCollision( "CameraCollision", *m_cameraManager, 0xff );
	m_cameraManager->RegisterConstraint( losConstarin );
	m_cameraManager->RegisterConstraint( collisionConstrain );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->ChangeCameraBehaviourTo( "PC", 0.f );
}

Scene_ProportionalController::~Scene_ProportionalController()
{
	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "Follow" );
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );
	m_cameraManager->DeleteCameraBehaviour( "PC" );

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

void Scene_ProportionalController::JustFinishedTransition()
{

}

void Scene_ProportionalController::BeginFrame()
{
	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );
}

void Scene_ProportionalController::EndFrame()
{

}

void Scene_ProportionalController::Update()
{
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	m_cameraManager->PreUpdate();

	TODO( "Camera Relative Controls - DISABLED" );
	// m_player->InformAboutCameraForward( m_cameraManager->GetCameraMatrixForInputReference().GetKColumn() );

	// Update Game Objects
	for( int i = 0; i < NUM_ENTITIES; i++ )
	{
		for each( GameObject* go in m_gameObjects[i] )
			go->Update( deltaSeconds );
	}

	// Update Camera Stuffs
	EnableDisableCameraConstrains();
	m_cameraManager->Update( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );

	m_cameraManager->PostUpdate();
}

void Scene_ProportionalController::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// DEBUG
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Render the Scene
	g_theRenderer->SetAmbientLight( m_ambientLight );

	Vector3 playerPosition = m_player->m_transform.GetWorldPosition();
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_scene, &playerPosition );
}

RaycastResult Scene_ProportionalController::Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance )
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

Vector3 Scene_ProportionalController::SphereCollision( Sphere const &sphere )
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

void Scene_ProportionalController::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_ProportionalController::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

void Scene_ProportionalController::ChangeCameraBehaviour()
{
	return;
}

void Scene_ProportionalController::EnableDisableCameraConstrains()
{
	XboxController &controller = g_theInput->m_controller[0];
	bool toggleConstrainButton = controller.m_xboxButtonStates[ XBOX_BUTTON_X ].keyJustPressed;

	if( toggleConstrainButton == true )
	{
		// Toggle the constraint
		m_constrainsActive = !m_constrainsActive;
		m_cameraManager->EnableConstraints( m_constrainsActive );

		// Debug Print
		float		yOffset;
		Rgba		constarinsColor;
		std::string constrainsActiveStr = Stringf( "Camera Constraints: ", m_constrainsActive );
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

void Scene_ProportionalController::DebugRenderTerrainNormalRaycast() const
{
	Vector3 raycastDirection = m_camera->m_cameraTransform.GetQuaternion().RotatePoint( Vector3::FRONT );
	RaycastResult result	 = m_terrain->Raycast( m_camera->m_cameraTransform.GetWorldPosition(), raycastDirection, 100.f, 0.01f );

	DebugRenderLineSegment( 0.f, result.impactPosition, RGBA_KHAKI_COLOR, result.impactPosition + result.impactNormal, RGBA_YELLOW_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
}


