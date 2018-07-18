#pragma once
#include "Level.hpp"
#include <thread>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/File.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"
#include "Game/World/Pipe.hpp"

using namespace tinyxml2;

void WriteTheGarbageFile( void * )
{
	File garbageFile;
	if( garbageFile.Open( "ThreadTest/garbage.txt", FILE_OPEN_MODE_APPEND ) == false )
		return;

	for( uint i = 0; i < 1200000; i++ )
	{
		std::string randomNumber = Stringf( "%f\n", GetRandomFloatZeroToOne() );
		garbageFile.Write( randomNumber );
	}

	garbageFile.Close();
}

void NonThreadedTestWork( Command &cmd )
{
	UNUSED( cmd );

	WriteTheGarbageFile( nullptr );
}

void ThreadedTestWork( Command &cmd )
{
	UNUSED( cmd );

	std::thread writerThread( WriteTheGarbageFile, nullptr );
	writerThread.detach();
}

void StartLoggingFromFile( char const *srcFilePath, int threadID )
{
	File sourceFile;
	sourceFile.Open( srcFilePath, FILE_OPEN_MODE_READ );
	if( sourceFile.IsOpen() )
	{
		uint		lineNum = 0;
		std::string lineStr;
		while( sourceFile.ReadNextLine( lineStr ) )
		{
			lineNum++;

			lineStr = Stringf( "[%d: %u] %s", threadID, lineNum, lineStr.c_str() );
			LogSystem::GetInstance()->LogPrintf( lineStr.c_str() );
		}

		sourceFile.Close();
	}
}

void LogBigTestCommand( Command &cmd )
{
	std::string fileName		= cmd.GetNextString();
	std::string threadCountStr	= cmd.GetNextString();
	
	int threadCount = 10;
	if( threadCountStr != "" )
		SetFromText( threadCount, threadCountStr.c_str() );

	for( int i = 0; i < threadCount; i++ )
	{
		std::thread readerThread( [ fileName, i ](){ StartLoggingFromFile( fileName.c_str(), i ); } );
		readerThread.detach();
	}
}

Level::Level( std::string definitionName, Robot &playerRobot )
	: m_pickBuffer( *g_theRenderer )
	, m_definition( *LevelDefinition::s_definitions[ definitionName ] )
	, m_playerRobot( playerRobot )
{
	CommandRegister( "non_threaded_test", NonThreadedTestWork );
	CommandRegister( "threaded_test", ThreadedTestWork );
	CommandRegister( "log_print_file_thread_count", LogBigTestCommand );
}

Level::~Level()
{
	delete m_tower;

	delete m_renderingPath;
	delete m_levelScene;

	DebugRendererShutdown();

	// Lights
	for( unsigned int i = 0; i < m_lightSources.size(); i++ )
		delete m_lightSources[i];
	m_lightSources.clear();

	// GameObject Pool
	for( unsigned int i = 0; i < m_allGameObjects.size(); i++ )
		delete m_allGameObjects[i];

	m_allGameObjects.clear();
	
	delete m_camera;
}

void Level::Startup()
{
	// Setup the UI Camera
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );

	// Setup the Orbitcamera
	m_camera = new OrbitCamera( Vector3::ZERO );
	// Set Color Targets
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	// Projection Matrix
	m_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f ); 
	// Orbit Camera
	m_camera->SetSphericalCoordinate( 10.f, -90.f, 90.f );
	// Skybox
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	m_lightSources.push_back( new Light( Vector3::ZERO ) );
	m_lightSources[0]->SetUpForPointLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	m_lightSources[0]->EnablePerimeterRotationAround( Vector3( 0.f, 0.f, 5.f ), 10.f );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, m_camera );

	// Battle Scene
	m_levelScene = new Scene();

	m_levelScene->AddLight( *m_lightSources[0] );

	m_levelScene->AddRenderable( *m_lightSources[0]->m_renderable );

	m_levelScene->AddCamera( *m_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );

	// Prepare the Tower
	m_tower = new Tower( Vector3::ZERO, m_definition.m_towerName );
	m_tower->SetFinishBlockAt( m_definition.m_spawnFinishAt );

	for( uint i = 0; i < m_tower->m_allBlocks.size(); i++ )
		m_levelScene->AddRenderable( *m_tower->m_allBlocks[i]->m_renderable );

	for( uint i = 0; i < m_tower->m_allPipes.size(); i++ )
		m_levelScene->AddRenderable( *m_tower->m_allPipes[i]->m_renderable );

	// Setup the Player Robot
	IntVector3 playerActualPos = m_definition.m_spawnPlayerOn + IntVector3::UP;		// Note: to spawn on top of that block, not at it!
	m_playerRobot.SetParentTower( *m_tower );
	m_playerRobot.SetPositionInTower( playerActualPos );

	m_levelScene->AddRenderable( *m_playerRobot.m_renderable );

	// Change the center of OrbitCamera
	Vector3 towerCenter = m_tower->m_transform.GetWorldPosition() + ( Vector3( m_tower->m_dimensionXYZ ) * 0.5f );
	m_camera->m_target = towerCenter;
}

void Level::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Level::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
	
	// If reached to final position
	IntVector3 posInTower	= m_playerRobot.GetPositionInTower();
	IntVector3 finishPos	= GetFinishPositionInTower();
	m_puzzleSolved			= (finishPos == posInTower);
	
	if( m_puzzleSolved )
		return;

	// Level::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Mouse click - start
	if( g_theInput->WasMousButtonJustPressed( MOUSE_BUTTON_LEFT ) )
	{
		// Note the start position on mouse
		m_mouseClickStartPos = g_theInput->GetMouseClientPosition();

		// Start of the drag gesture
		Block* clickedBlock = GetBlockFromMousePosition();

		// Set Drag Block
		if( clickedBlock != nullptr && clickedBlock->IsDraggable() )
		{
			Pipe	*blockOnPipe		= m_tower->GetAnchorPipeForBlock( *clickedBlock );
			Vector2  mouseInitialPos	= g_theInput->GetMouseClientPosition();
			Vector3  blockInitialPos	= Vector3( clickedBlock->GetMyPositionInTower() );
			
			m_dragBlock					= clickedBlock;
			m_dragData					= BlockDragData( blockOnPipe, mouseInitialPos, blockInitialPos );
		}
		else
			m_dragBlock = nullptr;
	}

	// Mouse click - ongoing (dragging gesture)
	if( g_theInput->IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
	{
		if( m_dragBlock != nullptr && m_dragData.anchorPipe != nullptr )
		{
			Vector2	currentMousePos	= g_theInput->GetMouseClientPosition();
			float	dragDistance	= GetDragDistanceOnPipe( currentMousePos, m_dragData );

			// Draw debug drag box - KHAKI
			Vector3 debugBoxPos		= Vector3( m_dragBlock->GetMyPositionInTower() ) + ( m_dragData.anchorPipe->m_forwardDirection * dragDistance );
			DebugRenderPoint( 0.f, 1.f, debugBoxPos, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

			m_dragData.endBlockPos	= debugBoxPos;
		}
	}

	// Mouse click - end
	if( g_theInput->WasMouseButtonJustReleased( MOUSE_BUTTON_LEFT ) )
	{
		// If it was a click
		Vector2 mousePosAtEnd		= g_theInput->GetMouseClientPosition();
		float	mouseMovedPixels	= ( mousePosAtEnd - m_mouseClickStartPos ).GetLength();
		if( mouseMovedPixels < 5 )
		{
			// Set Target Block - for move operation on Robot
			Block* clickedBlock = GetBlockFromMousePosition();
			if( clickedBlock != nullptr )
			{
				m_selectedBlock = clickedBlock;
				Block *targetBlock = m_tower->GetBlockOnTopOfMe( *clickedBlock );
				m_playerRobot.SetTargetBlock( *targetBlock );
			}
		}
		else
			m_selectedBlock = nullptr;

		// End of the drag operation
		// Put the dragable block at release position
		if( m_dragBlock != nullptr && m_dragData.anchorPipe != nullptr )
		{
			IntVector3	releasePosInTower	= IntVector3( m_dragData.endBlockPos + Vector3( 0.5f, 0.5f, 0.5f ));		// + Vec3(0.5f) because otherwise normal conversion will just do the floor operation
			IntVector3	startPosInTower		= IntVector3( m_dragData.startBlockPos );
			m_tower->SwapTwoBlocksAt( releasePosInTower, startPosInTower );
		}

		// Reset the Drag Block to nullptr
		m_dragBlock = nullptr;
	}

	// Show selected block
	if( m_selectedBlock != nullptr )
		m_selectedBlock->ObjectSelected();

	// Update Robot
	m_playerRobot.Update( deltaSeconds );

	// Camera Movement
	RotateTheCameraAccordingToPlayerInput( deltaSeconds );
	
	// Lights
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < m_lightSources.size(); i++ )
		m_lightSources[i]->Update( deltaSeconds );

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );
}

void Level::Render() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", m_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_levelScene );
	
	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) + " [ UP, DOWN ]" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);
	
	// Basis at Origin
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	DebugRendererRender();

	// If puzzle solved, show the message
	if( m_puzzleSolved )
		ShowPuzzleSolved();
}

double Level::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Level::RotateTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const rotationSpeed = 45.f;	// Degrees per seconds
	static float const altitudeSpeed = 20.f;

	float turnLeftAxis		= 0.f;
	float altitudeUpAxis	= 0.f;

	// Theta Change
	if( g_theInput->IsKeyPressed( 'A' ) )
		turnLeftAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'D' ) )
		turnLeftAxis -= 1.f;

	// Altitude Change
	if( g_theInput->IsKeyPressed( 'W' ) )
		altitudeUpAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'S' ) )
		altitudeUpAxis -= 1.f;

	float cameraRadius	 = m_camera->m_radius;
	float cameraAltitude = m_camera->m_altitude;
	float cameraRotation = m_camera->m_rotation;
	
	// Set Rotation & Altitude
	cameraRotation		+= deltaSeconds * rotationSpeed * -turnLeftAxis;
	cameraAltitude		+= deltaSeconds * altitudeSpeed * -altitudeUpAxis;
	cameraAltitude		 = ClampFloat( cameraAltitude, 30.f, 30.f + 90.f );

	m_camera->SetSphericalCoordinate( cameraRadius, cameraRotation, cameraAltitude );
}

void Level::ChnageLightAsPerInput(float deltaSeconds)
{
	// Ambient Light
	static float const ambientFactorChangeSpeed = 0.35f;		// Per seconds

	if (g_theInput->IsKeyPressed(UP))
	{
		m_ambientLight.w += ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}
	if (g_theInput->IsKeyPressed(DOWN))
	{
		m_ambientLight.w -= ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}

	g_theRenderer->SetAmbientLight( m_ambientLight );
}

Block* Level::GetBlockFromMousePosition()
{
	// Mouse Position
	Vector2 mousClientPos = g_theInput->GetMouseClientPosition();

	// Pick buffer
	m_pickBuffer.GeneratePickBuffer( *m_camera, *m_levelScene );
	uint		pickID				= m_pickBuffer.GetPickID( mousClientPos );
	GameObject *selectedGameObject	= GameObject::GetFromPickID( pickID );

	// If it is a Block
	if( selectedGameObject != nullptr && selectedGameObject->m_type == GAMEOBJECT_TYPE_BLOCK )
	{
		return (Block*)selectedGameObject;
	}

	return nullptr;
}

float Level::GetDragDistanceOnPipe( Vector2 const &mousePos, BlockDragData const &startDragData )
{
	// If Block is not on a pipe, it can't move
	if( startDragData.anchorPipe == nullptr )
		return 0.f;

	// Get pipe's forward direction in screen space
	Vector3 pipeStartPos		= startDragData.anchorPipe->m_startPosition;
 	Vector3 pipeDirection		= startDragData.anchorPipe->m_forwardDirection;

	Vector2 pipeStartOnScreen	= m_camera->GetScreenPositionFromWorld( pipeStartPos, 1.f );
 	Vector2 pipeDirEndOnScreen	= m_camera->GetScreenPositionFromWorld( pipeStartPos + pipeDirection, 1.f );
	Vector2 pipeDirOnScreen		= pipeDirEndOnScreen - pipeStartOnScreen;

	Vector2 mousePosThisFrame	= mousePos;
	Vector2 mouseDisplacement	= mousePosThisFrame - startDragData.startMousePos;
	mouseDisplacement.y			= -1.f * mouseDisplacement.y;						// Note: Top-left it Vec2::ZERO in screen space, so Y-Displacement is inverted

	// Project pipe direction on mouse's displacement
	float	pipeDirLength		= pipeDirOnScreen.GetLength();
	Vector2	pipeNormalizedDir	= pipeDirOnScreen.GetNormalized();
	float	projection			= Vector2::DotProduct( pipeNormalizedDir, mouseDisplacement );
	float	unitsMoved			= projection / pipeDirLength;
	
	DebugRender2DText( 0.f, Vector2(-850.f, 440.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf( "Units Moved: %f", unitsMoved ) );

	// Clamp the unitMoved so it doesn't go outside the boundary of pipe
	float	distFromStartOfPipe	= ( startDragData.startBlockPos - pipeStartPos ).GetLength();
	float	minMoveAllowed		= -distFromStartOfPipe;
	float	maxMoveAllowed		=  startDragData.anchorPipe->m_length - distFromStartOfPipe;
	float	unitsAllowedToMove	= ClampFloat( unitsMoved, minMoveAllowed, maxMoveAllowed );

	return unitsAllowedToMove;
}

IntVector3 Level::GetFinishPositionInTower() const
{
	return (m_definition.m_spawnFinishAt + IntVector3::UP);
}

void Level::ShowPuzzleSolved() const
{
	// Setup the renderer with uiCamera
	g_theRenderer->BindCamera( m_uiCamera );

	g_theRenderer->UseShader( nullptr );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	BitmapFont *font = g_theRenderer->CreateOrGetBitmapFont( "SquirrelFixedFont" );
	g_theRenderer->DrawAABB( m_uiDrawBounds, m_uiBackgroundColor );
	g_theRenderer->DrawTextInBox2D( "Puzzle Solved!", Vector2( 0.5f, 0.5f ), m_uiDrawBounds, 0.1f, RGBA_GREEN_COLOR, font, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "(Hit ESC to go back to level selection)", Vector2( 0.5f, 0.02f ), m_uiDrawBounds, 0.03f, RGBA_GREEN_COLOR, font, TEXT_DRAW_SHRINK_TO_FIT );;
}
