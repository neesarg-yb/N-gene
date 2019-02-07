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

	m_camera->RebuildMatrices();
}

void Scene_ProtoScene3D::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	Camera &camera = *m_camera->GetCamera();

	// Bind the camera
	g_theRenderer->BindCamera( &camera );
	
	// Do the camera cleanup operations
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	// camera.PreRender( *g_theRenderer );

	g_theRenderer->EnableLight( 0, *m_lights[0] );
	RenderBasis( 1.f );

	// camera.PostRender( *g_theRenderer );
}

void Scene_ProtoScene3D::ProcessInput( float deltaSeconds )
{
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );

	// Camera Rotation
	Vector2 mouseChange = g_theInput->GetMouseDelta();
	float const curentCamPitch = m_camera->GetPitchDegreesAboutY();

	m_camera->m_yawDegreesAboutZ	-= mouseChange.x * m_camRotationSpeed;
	m_camera->SetPitchDegreesAboutY( curentCamPitch + (mouseChange.y * m_camRotationSpeed) );

	// Camera Position
	float	const camYaw	 = m_camera->m_yawDegreesAboutZ;
	Vector3 const forwardDir = Vector3( CosDegree(camYaw), SinDegree(camYaw), 0.f );
	Vector3 const leftDir	 = Vector3( forwardDir.y * -1.f, forwardDir.x, 0.f );
	Vector3 const upDir		 = Vector3( 0.f, 0.f, 1.f );

	float forwardMovement = 0.f;
	float leftMovement = 0.f;
	float upMovement = 0.f;

	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += m_flySpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= m_flySpeed * deltaSeconds;

	Vector3 positionChange = (forwardDir * forwardMovement) + (leftDir * leftMovement) + (upDir * upMovement);
	m_camera->m_position += positionChange;
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

void Scene_ProtoScene3D::RenderBasis( float length ) const
{
	Vertex_3DPCU vBuffer[6];
	
	vBuffer[0].m_color		= RGBA_RED_COLOR;
	vBuffer[0].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[1].m_color		= RGBA_RED_COLOR;
	vBuffer[1].m_position	= Vector3( length, 0.f, 0.f );


	vBuffer[2].m_color		= RGBA_GREEN_COLOR;
	vBuffer[2].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[3].m_color		= RGBA_GREEN_COLOR;
	vBuffer[3].m_position	= Vector3( 0.f, length, 0.f );
	
	vBuffer[4].m_color		= RGBA_BLUE_COLOR;
	vBuffer[4].m_position	= Vector3( 0.f, 0.f, 0.f );
	vBuffer[5].m_color		= RGBA_BLUE_COLOR;
	vBuffer[5].m_position	= Vector3( 0.f, 0.f, length );

	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, 6, PRIMITIVE_LINES );
}
