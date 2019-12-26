#pragma once
#include "Scene_ProtoScene3D.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"
#include "Game/Camera System/Camera Behaviours/CB_ZoomCamera.hpp"

void SetReticlePosSs( Command &cmd )
{
	std::string const xOffsetStr = cmd.GetNextString();
	std::string const yOffsetStr = cmd.GetNextString();
	
	int xOffset = 0;
	if( xOffsetStr != "" )
		xOffset = ::atoi( xOffsetStr.c_str() );

	int yOffset = 0;
	if( yOffsetStr != "" )
		yOffset = ::atoi( yOffsetStr.c_str() );

	int const halfScreenWidth  = ( int ) ( Window::GetInstance()->GetWidth() * 0.5f );
	int const halfScreenHeight = ( int ) ( Window::GetInstance()->GetHeight() * 0.5f );

	if( abs( xOffset ) > halfScreenWidth )
	{
		xOffset = 0;
		ConsolePrintf( RGBA_RED_COLOR, "Error! Pass x-offset in range [-%d, %d]", halfScreenWidth, halfScreenWidth );
	}
	if( abs( yOffset ) > halfScreenHeight )
	{
		yOffset = 0;
		ConsolePrintf( RGBA_RED_COLOR, "Error! Pass y-offset in range [-%d, %d]", halfScreenHeight, halfScreenHeight );
	}

	Scene_ProtoScene3D::s_reticlePos.x = (float) xOffset;
	Scene_ProtoScene3D::s_reticlePos.y = (float) yOffset;
	ConsolePrintf( RGBA_GREEN_COLOR, "Reticle offset set to [%d, %d]", xOffset, yOffset );
}

Vector2 Scene_ProtoScene3D::s_reticlePos = /*Vector2::ZERO*/Vector2( 0.f/*60.f*/, -150.f );

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
	m_camera->SetPerspectiveCameraProjectionMatrix( m_initialFOV, Window::GetInstance()->GetAspectRatio(), m_cameraNear, m_cameraFar );
	m_camera->EnableShadowMap();
	m_camera->RenderDebugObjects( true );
	m_scene->AddCamera( *m_camera );

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );
	m_cameraManager->SetAnchor( nullptr );

	// Camera Behavior
	CameraBehaviour* freelookBehaviour	= new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", m_cameraManager, USE_KEYBOARD_MOUSE_FL );
	m_cameraManager->AddNewCameraBehaviour( freelookBehaviour );
	m_cameraManager->SetActiveCameraBehaviourTo( "FreeLook" );					// MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR
	m_zoomCameraActive = false;

	// Loading Models
	Vector3 snowMikuPosition = Vector3( 0.f, -3.f, 8.f );						// SNOW MIKU
	Vector3 snowMikuRotation = Vector3( 0.f, 180.f, 0.f );
	m_snowMiku		= new Renderable( snowMikuPosition, snowMikuRotation, Vector3::ONE_ALL );
	bool mikuLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\snow_miku\\ROOMITEMS011_ALL.obj", *m_snowMiku );
	GUARANTEE_RECOVERABLE( mikuLoaded, "Snow Miku obj model loading FAILED!" );
	
	Vector3 spaceshipPosition = Vector3( 15.f, -3.f, 21.f );
	Vector3 spaceshipRotation = Vector3( 0.f, 180.f, 0.f );
	m_spaceship		= new Renderable( spaceshipPosition, spaceshipRotation, Vector3::ONE_ALL );
	bool shipLoaded = ModelLoader::LoadObjectModelFromPath( "Data\\Models\\scifi_fighter_mk6\\scifi_fighter_mk6.obj", *m_spaceship );
	GUARANTEE_RECOVERABLE( shipLoaded, "Spaceship obj model loading FAILED" );

	if( mikuLoaded )
		m_scene->AddRenderable( *m_snowMiku );
	if( shipLoaded )
		m_scene->AddRenderable( *m_spaceship );

	// Test Cube
	Mesh *cubeMesh = MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3( -5.f, 0.f, 5.f), RGBA_WHITE_COLOR, AABB2::ONE_BY_ONE, AABB2::ONE_BY_ONE, AABB2::ONE_BY_ONE );
	Material *testMaterial = Material::CreateNewFromFile( "Data\\Materials\\A01TestCube.material" );
	m_testCubeRenderable = new Renderable( cubeMesh, testMaterial );

	m_scene->AddRenderable( *m_testCubeRenderable );

	Vector3 const zoomCamOffset = Vector3(1.0f, 2.0f, -1.3f)/*Vector3::ZERO*/;
	m_zoomCameraBehavior = new CB_ZoomCamera( Vector3::ZERO, 60.f, "ZoomCamera", m_cameraManager );
	m_zoomCameraBehavior->SetCameraOffsetFromReference( zoomCamOffset );
	
	m_cameraManager->AddNewCameraBehaviour( m_zoomCameraBehavior );
	m_cameraManager->SetActiveCameraBehaviourTo( "ZoomCamera" );

	m_zoomCameraBehavior->SetReticleOffset( IntVector2( s_reticlePos ) );
	m_zoomCameraActive = true;

	// Debug Camera
	Vector3 const debugCamStartPos = Vector3( 3.f, 4.f, -22.f );
	m_debugCBFreeLook = new CB_FreeLook( 7.f, 35.f, -90.f, 90.f, "DebugFreeLook", nullptr, USE_CONTROLLER_FL );
	m_debugCamera = new DebugCamera( m_debugCBFreeLook, g_theInput );
	m_debugCamera->SetPerspectiveCameraProjectionMatrix( 60.f, Window::GetInstance()->GetAspectRatio(), 0.1f, 1000.f );
	m_debugCamera->SetCameraPositionTo( debugCamStartPos );
	m_debugCamera->LookAt( m_debugCamera->m_cameraTransform.GetWorldPosition(), Vector3::ZERO, Vector3::UP );

	m_targetPointWs = snowMikuPosition;

	m_scene->AddCamera( *m_debugCamera );

	// Reticle
	CommandRegister( "reticle_pos", SetReticlePosSs );
}

Scene_ProtoScene3D::~Scene_ProtoScene3D()
{
	// Debug Camera
	delete m_debugCamera;
	m_debugCamera = nullptr;

	// Camera Behaviour
	m_cameraManager->DeleteCameraBehaviour( "ZoomCamera" );
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

	// Obj Models
	delete m_snowMiku;
	m_snowMiku = nullptr;

	delete m_spaceship;
	m_spaceship = nullptr;
}

void Scene_ProtoScene3D::JustFinishedTransition()
{
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
	g_theInput->ShowCursor( false );
}

void Scene_ProtoScene3D::BeginFrame()
{
	PROFILE_SCOPE_FUNCTION();

	m_debugCamera->Update();

	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );

	DebugRender2DText( 0.f, Vector2( -850.f, 420.f ), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, "FreeLook: Mouse & WASD-QE: to move around." );
}

void Scene_ProtoScene3D::EndFrame()
{
	m_newTargetJustSpawnned = false;
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

	// Target for reticle
	SpawnTargetOnSpaceBar();
	if( m_newTargetJustSpawnned )
		m_zoomCameraBehavior->LookAtTargetPosition( m_targetPointWs );

	UpdateZoomCameraYawOffset( deltaSeconds );

	// Update Camera Stuffs
	CheckSwitchCameraBehavior();
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

	// Target Ws
	RenderTarget();
	DebugRenderZoomCamera();

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	m_renderingPath->RenderScene( *m_scene );
	m_renderingPath->RenderSceneForCamera( *m_debugCamera, *m_scene, nullptr );

	if( m_enableDebugOverlay )
		m_debugCamera->RenderAsFulscreenOverlay();
	else
		m_debugCamera->RenderAsMiniOverlay();
}

void Scene_ProtoScene3D::CheckSwitchCameraBehavior()
{
	if( g_theInput->WasKeyJustPressed( 'O' ) )
		m_enableDebugOverlay = !m_enableDebugOverlay;

	std::string debChangeOverlayText = Stringf( "[O] Change Debug Overlay");
	DebugRender2DText( 0.f, Vector2( -850.f, 440.f ), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, debChangeOverlayText.c_str() );

	std::string debChangeCamText = Stringf( "[C] Change Camera to %s", ( m_zoomCameraActive ? "FreeLook" : "ZoomCam" ) );
	DebugRender2DText( 0.f, Vector2( -850.f, 460.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, debChangeCamText.c_str() );

	if( !g_theInput->WasKeyJustPressed( 'C' ) )
		return;

	std::string switchToBehaviour = m_zoomCameraActive ? "FreeLook" : "ZoomCamera";
	m_cameraManager->SetActiveCameraBehaviourTo( switchToBehaviour );

	m_zoomCameraActive = !m_zoomCameraActive;
}

void Scene_ProtoScene3D::SpawnTargetOnSpaceBar()
{
	bool spaceBarJustPressed = g_theInput->WasKeyJustPressed( SPACE );
	bool aKeyJustPressed = g_theInput->m_controller->m_xboxButtonStates[XBOX_BUTTON_A].keyJustPressed;

	if( !spaceBarJustPressed && !aKeyJustPressed )
		return;

	m_targetPointWs = m_debugCamera->m_cameraTransform.GetWorldPosition();
	m_newTargetJustSpawnned = true;

	// Reset extra yaw rotation of ZoomCamera
	m_zoomCameraBehavior->SetCameraYawExtraRotation( 0.f );
}

void Scene_ProtoScene3D::UpdateZoomCameraYawOffset( float deltaSeconds )
{
	float const yawExtraRotSpeed = 5.f; // Degrees per seconds

	float curYawExtraRot = m_zoomCameraBehavior->GetCameraYawExtraRotation();
	float deltaYawRot = 0.f;

	// Rotate right
	if( g_theInput->IsKeyPressed( 'K' ) )
		deltaYawRot += yawExtraRotSpeed * deltaSeconds;

	// Rotate left
	if( g_theInput->IsKeyPressed( 'H' ) )
		deltaYawRot -= yawExtraRotSpeed * deltaSeconds;

	curYawExtraRot += deltaYawRot;
	m_zoomCameraBehavior->SetCameraYawExtraRotation( curYawExtraRot );

	std::string debExtraYawRotText = Stringf( "Extra yaw rotation = %.2f", curYawExtraRot );
	DebugRender2DText( 0.f, Vector2( -850.f, 400.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[H] & [K] adds extra yaw rotation to ZoomCamera" );
	DebugRender2DText( 0.f, Vector2( -850.f, 380.f ), 15.f, RGBA_RED_COLOR, RGBA_RED_COLOR, debExtraYawRotText.c_str() );
}

void Scene_ProtoScene3D::RenderTarget() const
{
	DebugRenderPoint( 0.0f, 1.5f, m_targetPointWs, RGBA_RED_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_XRAY );
}

void Scene_ProtoScene3D::DebugRenderZoomCamera() const
{
	// Reference Direction
	Transform const refTransform = m_zoomCameraBehavior->GetReferenceTransform();
	Vector3 const refPosWs = refTransform.GetWorldPosition();
	Vector3 const refDirWs = refTransform.GetQuaternion().GetFront();
	DebugRenderVector( 0.f, refPosWs, refDirWs * 2.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Laser line to target
	Matrix44 const camTransformMat = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	Vector3 const cameraForward = camTransformMat.GetKColumn();
	Vector3 const cameraPos = camTransformMat.GetTColumn();
	DebugRenderLineSegment( 0.f, cameraPos, RGBA_WHITE_COLOR, cameraPos + ( refDirWs * 100.f ), RGBA_ORANGE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Reticle
	DebugRender2DRound( 0.f, s_reticlePos, 3.5f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR );
}

Vector3 Scene_ProtoScene3D::GetDirectionWsFromReticleOffsetX( float offsetFromCenter ) const
{
	IntVector2 const screenDim = Window::GetInstance()->GetDimensions();

	Vector2 const screenPos		= Vector2( screenDim.x * 0.5f + offsetFromCenter, screenDim.y * 0.5f );
	Vector3 const screenPointWs	= m_camera->GetWorldPositionFromScreen( screenPos, -1.f );

	Matrix44 const camMat44 = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	Vector3  const camPosWs = camMat44.GetTColumn();

	Vector3  const reticleDirectionWs = ( screenPointWs - camPosWs ).GetNormalized();

	return reticleDirectionWs;
}

Vector3 Scene_ProtoScene3D::GetDirectionWsFromReticleOffsetY( float offsetFromCenter ) const
{
	IntVector2 const screenDim = Window::GetInstance()->GetDimensions();

	Vector2 const screenPos		= Vector2( screenDim.x * 0.5f, screenDim.y * 0.5f + offsetFromCenter );
	Vector3 const screenPointWs	= m_camera->GetWorldPositionFromScreen( screenPos, -1.f );

	Matrix44 const camMat44 = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	Vector3  const camPosWs = camMat44.GetTColumn();

	Vector3  const reticleDirectionWs = ( screenPointWs - camPosWs ).GetNormalized();

	return reticleDirectionWs;
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
