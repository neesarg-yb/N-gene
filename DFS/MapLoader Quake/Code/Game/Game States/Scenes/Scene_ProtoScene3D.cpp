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
	Light *directionalLight1 = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( 40.f, -45.f, 0.f ) );
	directionalLight1->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f) );
	directionalLight1->UsesShadowMap( true );
	AddNewLightToScene( directionalLight1 );
	
	Light *directionalLight2 = new Light( Vector3( 10.f, 10.f, 0.f ), Vector3( -40.f, -45.f, 0.f ) );
	directionalLight2->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f), RGBA_KHAKI_COLOR );
	directionalLight2->UsesShadowMap( true );
	AddNewLightToScene( directionalLight2 );
	
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

	// Loading Models
	Vector3 snowMikuPosition = Vector3( -5.f, -3.f, 20.f );						// SNOW MIKU
	Vector3 snowMikuRotation = Vector3( 0.f, 180.f, 0.f );
	Renderable* snowMiku	= new Renderable( snowMikuPosition, snowMikuRotation, Vector3::ONE_ALL );
	bool mikuLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\snow_miku\\ROOMITEMS011_ALL.obj", *snowMiku );
	GUARANTEE_RECOVERABLE( mikuLoaded, "Snow Miku obj model loading FAILED!" );
	
	Vector3 spaceshipPosition = Vector3( 5.f, -3.f, 21.f );
	Vector3 spaceshipRotation = Vector3( 0.f, 180.f, 0.f );
	Renderable* spaceship	= new Renderable( spaceshipPosition, spaceshipRotation, Vector3::ONE_ALL );
	bool shipLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\scifi_fighter_mk6\\scifi_fighter_mk6.obj", *spaceship );
	GUARANTEE_RECOVERABLE( shipLoaded, "Spaceship obj model loading FAILED" );

	AddNewRenderableToScene( snowMiku );
	AddNewRenderableToScene( spaceship );

	// TESTING MAP FILE LOADING
//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\START.MAP" );
//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\B_BARREL.MAP" );
	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\B_KEY1.MAP" );
//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\Test1.MAP" );

	for( int e = 0; e < m_parsedMap->m_entities.size(); e++ )
	{
		MapEntity const &entity = m_parsedMap->m_entities[e];
		
		if( entity.m_className != "worldspawn" )
			continue;

		for( int g = 0; g < entity.GetBrushCount(); g++ )
		{
			Renderable *geometryRenderable = entity.ConstructRenderableForBrushAtIndex(g);

			if( geometryRenderable != nullptr )
				AddNewRenderableToScene( geometryRenderable );
		}
	}
}

Scene_ProtoScene3D::~Scene_ProtoScene3D()
{
	// MAP Parser
	if( m_parsedMap != nullptr )
	{
		delete m_parsedMap;
		m_parsedMap = nullptr;
	}

	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "FreeLook" );

	// Camera Manager
	delete m_cameraManager;
	m_cameraManager = nullptr;

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

	while( m_renderables.size() > 0 )
	{
		std::swap( m_renderables.front(), m_renderables.back() );
		
		delete m_renderables.back();
		m_renderables.back() = nullptr;

		m_renderables.pop_back();
	}

}

void Scene_ProtoScene3D::JustFinishedTransition()
{
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
	g_theInput->ShowCursor( false );
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
	
	m_cameraManager->PreUpdate();
	
	// Update Camera Stuffs
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();
	m_cameraManager->Update( deltaSeconds );

	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
	{
		g_theInput->ShowCursor( true );
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	}

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

	// Debug Renderer
	DebugRendererLateRender( m_camera );
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

void Scene_ProtoScene3D::AddNewRenderableToScene( Renderable *renderable )
{
	m_renderables.push_back( renderable );
	m_scene->AddRenderable( *renderable );
}
