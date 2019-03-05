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

	// Point light on camera
	Light *pointLight = new Light( Vector3::ZERO, Vector3::ZERO );
	pointLight->SetUpForPointLight( 300.f );
	pointLight->UsesShadowMap( false );
	m_lights.push_back( pointLight );
	m_scene->AddLight( *pointLight );

	// Directional lights
	Light *directionalLight1 = new Light( Vector3( 10.f, 0.f, 0.f ), Vector3( 45.f, -45.f, 0.f ) );
	directionalLight1->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f), RGBA_RED_COLOR );
	directionalLight1->UsesShadowMap( false );
	AddNewLightToScene( directionalLight1 );

	Light *directionalLight2 = new Light( Vector3( -10.f, 0.f, 0.f ), Vector3( 45.f, 45.f, 0.f ) );
	directionalLight2->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f), RGBA_BLUE_COLOR );
	directionalLight2->UsesShadowMap( false );
	AddNewLightToScene( directionalLight2 );

	Light *directionalLight3 = new Light( Vector3( 10.f, -20.f, 20.f ), Vector3( -45.f, 225.f, 0.f ) );
	directionalLight3->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f), RGBA_GREEN_COLOR );
	directionalLight3->UsesShadowMap( false );
	AddNewLightToScene( directionalLight3 );

	Light *directionalLight4 = new Light( Vector3( -10.f, -20.f, 20.f ), Vector3( -45.f, 135.f, 0.f ) );
	directionalLight4->SetUpForDirectionalLight( 50.f, Vector3( 1.f, 0.f, 0.f), RGBA_YELLOW_COLOR );
	directionalLight4->UsesShadowMap( false );
	AddNewLightToScene( directionalLight4 );
	
	// Setting up the Camera
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( g_theRenderer->GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );
	m_camera->EnableShadowMap();
	m_camera->RenderDebugObjects( true );
	m_scene->AddCamera( *m_camera );

	// Point light follows the camera
	pointLight->m_transform.SetParentAs( &m_camera->m_cameraTransform );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );
	m_cameraManager->SetAnchor( nullptr );

	// Camera Behaviour
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 100.f, 40.f, -85.f, 85.f, "FreeLook", m_cameraManager, USE_KEYBOARD_MOUSE_FL );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );
	m_cameraManager->ChangeCameraBehaviourTo( "FreeLook", 0.f );				// MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR

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

	AddTestConvexPolyhedronToScene();
//	AddTestQuakeMapToScene();
}

Scene_ProtoScene3D::~Scene_ProtoScene3D()
{
	if( m_testHedronRenderable != nullptr )
	{
		delete m_testHedronRenderable;
		m_testHedronRenderable = nullptr;
	}

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

void Scene_ProtoScene3D::AddTestConvexPolyhedronToScene()
{
	// TESTING CONVEX POLYHEDRON
	Plane3 plane1( Vector3::UP,				4.5f );
	Plane3 plane2( Vector3::UP * -1.f,		4.5f );
	Plane3 plane3( Vector3::RIGHT,			4.5f );
	Plane3 plane4( Vector3::RIGHT * -1.f,	4.5f );
	Plane3 plane5( Vector3::FRONT,			4.5f );
	Plane3 plane6( Vector3::FRONT * -1.f,	4.5f );

	m_testHedron.AddPlane( plane1 );
	m_testHedron.AddPlane( plane2 );
	m_testHedron.AddPlane( plane3 );
	m_testHedron.AddPlane( plane4 );
	m_testHedron.AddPlane( plane5 );
	m_testHedron.AddPlane( plane6 );

// 	// Adding extra plane near the top-back edge
// 	Plane3 plane7( Vector3( 0.f, 1.f, 1.f).GetNormalized(), 3.f );
// 	m_testHedron.AddPlane( plane7 );

	m_testHedron.SetFaceWindingOrder( WIND_COUNTER_CLOCKWISE );
	m_testHedron.Rebuild( 0.01f );

	// TESTING PLYHEDRON'S RENDERABLE
	Vector3		 testMeshLocation	= Vector3( 15.f, 0.f, 0.f );
	Mesh		*testHedronMesh		= m_testHedron.ConstructMesh( RGBA_WHITE_COLOR );
	Material	*testHedronMaterial	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_testHedronRenderable			= new Renderable( Transform(testMeshLocation, Vector3::ZERO, Vector3::ONE_ALL), testHedronMesh, testHedronMaterial );

	m_scene->AddRenderable( *m_testHedronRenderable );
}

void Scene_ProtoScene3D::AddTestQuakeMapToScene()
{
	// TESTING MAP FILE LOADING
	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\DM3.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\B_KEY1.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\START.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\END.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\e2m10.map" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\E3M5.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\B_BARREL.MAP" );
	//	m_parsedMap = MapParser::LoadFromFile( "Data\\MAP\\Test1.MAP" );
	
	for( int e = 0; e < m_parsedMap->m_entities.size(); e++ )
	{
		MapEntity const &entity = m_parsedMap->m_entities[e];
		for( int g = 0; g < entity.GetBrushCount(); g++ )
		{
			Renderable *geometryRenderable = entity.ConstructRenderableForBrushAtIndex(g);

			if( geometryRenderable != nullptr )
				AddNewRenderableToScene( geometryRenderable );
		}

		if( entity.m_className == "info_player_start" )
		{
			std::string startPosStr  = entity.m_properties.at( "origin" );
			Strings posFloatsStrings = SplitIntoStringsByDelimiter( startPosStr, ' ' );

			if( posFloatsStrings.size() == 3 )
			{
				float x, y, z;
				::SetFromText( x, posFloatsStrings[0].c_str() );
				::SetFromText( z, posFloatsStrings[1].c_str() );
				::SetFromText( y, posFloatsStrings[2].c_str() );

				m_camera->m_cameraTransform.SetPosition( Vector3(x, y, z) );
			}
		}
	}
}
