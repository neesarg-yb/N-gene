#pragma once
#include "Scene_CameraStateAverage.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"

Scene_CameraStateAverage::Scene_CameraStateAverage( Clock const *parentClock )
	: GameState( "CAMERA STATE AVERAGE", parentClock )
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

	// Camera Manager
	m_cameraManager = new CameraManager( *m_camera, *g_theInput, 0.1f );

	// Camera Behaviour
	CameraBehaviour* freelookBehavior = new CB_FreeLook( 10.f, 40.f, -60.f, 60.f, "FreeLook", m_cameraManager, USE_KEYBOARD_MOUSE_FL );
	m_cameraManager->AddNewCameraBehaviour( freelookBehavior );

	// Activate the behavior [MUST HAPPEN AFTER ADDING ALL CONTRAINTS TO BEHAVIOUR]
	m_cameraManager->SetActiveCameraBehaviourTo( "FreeLook" );
}

Scene_CameraStateAverage::~Scene_CameraStateAverage()
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

void Scene_CameraStateAverage::JustFinishedTransition()
{
	g_theInput->ShowCursor( false );
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
}

void Scene_CameraStateAverage::BeginFrame()
{
	// Update Debug Renderer Objects
	DebugRendererBeginFrame( m_clock );
}

void Scene_CameraStateAverage::EndFrame()
{

}

void Scene_CameraStateAverage::Update()
{
	m_cameraManager->PreUpdate();


	// Update Game Objects
	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	// Update Camera Stuffs
	m_cameraManager->Update( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::SPACE ) )
		AddCurrentCameraStateToHistoryForAverage();

	m_cameraManager->PostUpdate();


	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
	{
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		g_theInput->ShowCursor( true );

		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	}
}

void Scene_CameraStateAverage::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// Ambient Light
	g_theRenderer->SetAmbientLight( m_ambientLight );

	// Render the scene
	m_renderingPath->RenderScene( *m_scene );

	// Render the debug objects
	DebugRenderHowToUseInstructions();
	DebugRenderCameraStateHistory();
	DebugRenderCameraStateAverage();
	DebugRenderCameraOrientationCompare();

	DebugRendererLateRender( m_camera );
}

void Scene_CameraStateAverage::AddCurrentCameraStateToHistoryForAverage()
{
	m_camStateHistory.AddNewEntry( m_cameraManager->GetCurrentCameraState() );
}

void Scene_CameraStateAverage::DebugRenderHowToUseInstructions() const
{
	std::string controlsInfo1		= "Keyboard-Mouse to fly around";
	std::string controlsInfo2		= "Average basis shows up once the history is full!";
	std::string spaceFunctionInfo	= "SPACE    : add current camera state into the history.";
	std::string compareFunctionInfo	= "C BUTTON : compare all the bases at the position of the avg!";
	std::string currentEntriesInfo	= Stringf( "Entries in History = %d/%d", m_camStateHistory.GetCurrentCountOfEntries(), m_historyLength );

	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_BLACK_COLOR,  RGBA_BLACK_COLOR,  controlsInfo1.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 440.f), 15.f, RGBA_BLACK_COLOR,  RGBA_BLACK_COLOR,  controlsInfo2.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, spaceFunctionInfo.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, compareFunctionInfo.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 380.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, currentEntriesInfo.c_str() );
}

void Scene_CameraStateAverage::DebugRenderCameraStateHistory() const
{
	int totalEntries = m_camStateHistory.GetCurrentCountOfEntries();
	for( int i = totalEntries-1; i >= 0; i-- )
	{
		CameraState camStateEntry = m_camStateHistory.GetRecentEntry( i );

		// Draw the camera state as basis
		Matrix44 cameraMatrixOfEntry = camStateEntry.GetTransformMatrix();
		DebugRenderBasis( 0.f, cameraMatrixOfEntry, m_historyStateTintColor, m_historyStateTintColor, DEBUG_RENDER_XRAY );

		// Label the number of drawn camera state
		Matrix44 activeCamMatrix = m_camera->m_cameraTransform.GetWorldTransformMatrix();
		DebugRenderTag( 0.f, m_historyIndexLabelSize, cameraMatrixOfEntry.GetTColumn() + cameraMatrixOfEntry.GetIColumn(), activeCamMatrix.GetJColumn(), activeCamMatrix.GetIColumn(), RGBA_RED_COLOR,   RGBA_RED_COLOR, Stringf("X%d", totalEntries - i - 1) );
		DebugRenderTag( 0.f, m_historyIndexLabelSize, cameraMatrixOfEntry.GetTColumn() + cameraMatrixOfEntry.GetJColumn(), activeCamMatrix.GetJColumn(), activeCamMatrix.GetIColumn(), RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("Y%d", totalEntries - i - 1) );
		DebugRenderTag( 0.f, m_historyIndexLabelSize, cameraMatrixOfEntry.GetTColumn() + cameraMatrixOfEntry.GetKColumn(), activeCamMatrix.GetJColumn(), activeCamMatrix.GetIColumn(), RGBA_BLUE_COLOR,  RGBA_BLUE_COLOR, Stringf("Z%d", totalEntries - i - 1) );
	}
}

void Scene_CameraStateAverage::DebugRenderCameraStateAverage() const
{
	// Render only if the history is full
	if( m_camStateHistory.GetCurrentCountOfEntries() != m_historyLength )
		return;

	CameraState averageCamState = m_camStateHistory.GetAverageOfRecentEntries( m_historyLength );

	// Draw the camera state as basis
	Matrix44 cameraMatrixOfEntry = averageCamState.GetTransformMatrix();
	DebugRenderBasis( 0.f, cameraMatrixOfEntry, m_historyAverageTintColor, m_historyAverageTintColor, DEBUG_RENDER_XRAY );

	// Label the number of drawn camera state
	Matrix44 activeCamMatrix = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	DebugRenderTag( 0.f, m_historyIndexLabelSize, cameraMatrixOfEntry.GetTColumn() + cameraMatrixOfEntry.GetKColumn(), activeCamMatrix.GetJColumn(), activeCamMatrix.GetIColumn(), m_historyAverageTintColor, m_historyAverageTintColor, Stringf("AVG(%d)", m_historyLength) );
}

void Scene_CameraStateAverage::DebugRenderCameraOrientationCompare() const
{
	// Only when C key is pressed
	if( g_theInput->IsKeyPressed( 'C' ) == false )
		return;

	// Get average camera state
	int totalEntries = m_camStateHistory.GetCurrentCountOfEntries();
	CameraState averageCamState = m_camStateHistory.GetAverageOfRecentEntries( totalEntries );
	Matrix44 averageCamStateMat = averageCamState.GetTransformMatrix();

	// For each entries in the history
	for( int i = totalEntries-1; i >= 0; i-- )
	{
		CameraState camStateEntry = m_camStateHistory.GetRecentEntry( i );

		// Draw basis at the position of avg. camera state
		Matrix44 cameraMatrixOfEntry = camStateEntry.GetTransformMatrix();
		cameraMatrixOfEntry.SetTColumn( averageCamStateMat.GetTColumn() );
		DebugRenderBasis( 0.f, cameraMatrixOfEntry, m_historyStateTintColor, m_historyStateTintColor, DEBUG_RENDER_XRAY );
	}
}

void Scene_CameraStateAverage::AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType )
{
	// Add game object which gets updated every frame
	m_gameObjects[ entityType ].push_back( go );

	// Add its Renderable(s) to the Scene
	go->AddRenderablesToScene( *m_scene );
}

void Scene_CameraStateAverage::AddNewLightToScene( Light *light )
{
	// Add to list, so we can delete it at deconstruction
	m_lights.push_back( light );

	// Add light to scene
	m_scene->AddLight( *light );

	// Add its renderable
	m_scene->AddRenderable( *light->m_renderable );
}

