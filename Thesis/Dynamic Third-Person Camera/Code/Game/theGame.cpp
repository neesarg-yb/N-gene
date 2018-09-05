#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Game/theApp.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"
#include "Game/World/LevelDefinition.hpp"
#include "Engine/Math/Quaternions.hpp"

void EchoTestCommand( Command& cmd )
{
	ConsolePrintf( "%s", cmd.GetNextString().c_str() );
}

void ShowAllLogTags( Command& cmd )
{
	UNUSED( cmd );
	LogSystem::GetInstance()->ShowAllTags();
}

void HideAllLogTags( Command& cmd )
{
	UNUSED( cmd );
	LogSystem::GetInstance()->HideAllTags();
}

void ShowLogTag( Command& cmd )
{
	std::string tagName = cmd.GetNextString();
	LogSystem::GetInstance()->ShowTag( tagName );
}

void HideLogTag( Command& cmd )
{
	std::string tagName = cmd.GetNextString();
	LogSystem::GetInstance()->HideTag( tagName );
}

theGame::theGame()
{
	// Set global variable
	g_theGame = this;

	m_lastFramesTime = GetCurrentTimeSeconds();

	// Fonts for loading screen
	m_textBmpFont = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	// Camera Setup
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_gameCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho

	// TESTING QUATERNIONS
	// Test 1
	Quaternions rotateAroundX_Q	= Quaternions( Vector3::FRONT, 45.f );
	Vector3		rotatedVec1		= rotateAroundX_Q.RotatePoint( Vector3::UP );
	Matrix44	qRotate1_M		= rotateAroundX_Q.GetAsMatrix44();
	Vector3		eulerQRotate1_M	= qRotate1_M.GetEulerRotation();

	// Test 2
	Matrix44 rotateAroundX_M; 
	rotateAroundX_M.RotateDegrees3D( Vector3( 0.f, 0.f , 45.f) );
	Vector3	rotatedVec2				= rotateAroundX_M.Multiply( Vector3::UP, 0.f );
	Vector3 eulerRotateAroundX_M	= rotateAroundX_M.GetEulerRotation();

	// Test 3 - From Euler
	Vector3 eulerRotationZXY_Test3	= Vector3( 45.f, 90.f, 45.f);				// Roll counter clock wise => look up => turn right
	// My Rotation Matrix applies y & z rotation as left-hand-rule; where quaternions applies it as right-hand-rule => Multiply with -1.f
	Quaternions test3_Q				= Quaternions::FromEuler( eulerRotationZXY_Test3.x, -1.f * eulerRotationZXY_Test3.y, -1.f * eulerRotationZXY_Test3.z );
	Matrix44	test3_M				= Matrix44();
	test3_M.RotateDegrees3D( eulerRotationZXY_Test3 );

	Vector3 test3Vec_M = Vector3::FRONT;
	Vector3 test3Vec_Q = Vector3::FRONT;
	test3Vec_M = test3_M.Multiply( test3Vec_M, 0.f );
	test3Vec_Q = test3_Q.RotatePoint( test3Vec_Q );
	

	// Test 4 - From Matrix
	Matrix44	rotationMatFromEuler = test3_M;
	Quaternions	qFromMat			 = Quaternions::FromMatrix( rotationMatFromEuler );		// Is (qFromMat == test3_Q) ?? => YES!
	Vector3		sameAsTest3Vec_Q	 = Vector3::FRONT;
	sameAsTest3Vec_Q				 = qFromMat.RotatePoint( sameAsTest3Vec_Q );
	TODO( "sameAsTest3Vec_Q's z-axis is off.. compared to test3Vec_Q" );
}

theGame::~theGame()
{
	if( m_currentLevel != nullptr )
		delete m_currentLevel;

	delete m_playerRobot;

	LevelDefinition::DeleteAllDefinitions();
	TowerDefinition::DeleteAllDefinitions();
	BlockDefinition::DeleteAllDefinitions();

	delete m_textBmpFont;
}

void theGame::Startup()
{
	// Render Loading Screen
	g_theRenderer->BeginFrame();
	RenderLoadingScreen();
	g_theRenderer->EndFrame();

	// Console stuffs
	CommandRegister( "echo", EchoTestCommand );
	CommandRegister( "log_show_all", ShowAllLogTags );
	CommandRegister( "log_hide_all", HideAllLogTags );
	CommandRegister( "log_show_tag", ShowLogTag );
	CommandRegister( "log_hide_tag", HideLogTag );
	ConsolePrintf( RGBA_GREEN_COLOR, "%i Hello World!", 1 );

	// Seting up the Attract Menu
	m_attractMenu = new UIMenu( *g_theInput, *g_theRenderer, AABB2( 0.45f, 0.45f, 0.55f, 0.55f ) );

	m_attractMenu->AddNewMenuAction( MenuAction( "Quit", &quitStdFunc ) );
	m_attractMenu->AddNewMenuAction( MenuAction( "Start", &startStdFunc ) );
	m_attractMenu->m_selectionIndex = 1;


	// Load All Definitions
	BlockDefinition::LoadAllDefinitions( "Data\\Definitions\\Blocks.xml" );

	TowerDefinition::LoadDefinition( "Data\\Definitions\\Tower1.xml" );
	TowerDefinition::LoadDefinition( "Data\\Definitions\\Tower2.xml" );
	TowerDefinition::LoadDefinition( "Data\\Definitions\\Tower3.xml" );
	
	LevelDefinition::LoadDefinition( "Data\\Definitions\\Level1.xml" );
	LevelDefinition::LoadDefinition( "Data\\Definitions\\Level2.xml" );
	LevelDefinition::LoadDefinition( "Data\\Definitions\\Level3.xml" );

	// Setup the Robot
	m_playerRobot = new Robot( IntVector3::ZERO, nullptr );

	// Setup the LevelSelection UI
	m_levelSelectionMenu = new UIMenu( *g_theInput, *g_theRenderer, AABB2( 0.45f, 0.42f, 0.55f, 0.55f ) );
	for( LevelDefinitionMap::iterator it = LevelDefinition::s_definitions.begin(); it != LevelDefinition::s_definitions.end(); it++ )
	{
		m_levelSelectionMenu->AddNewMenuAction( MenuAction(it->first.c_str(), &levelSelectedStdFunc) );
	}
}

void theGame::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	if( m_currentGameState == LEVEL )
		m_currentLevel->BeginFrame();
}

void theGame::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	if( m_currentGameState == LEVEL )
		m_currentLevel->EndFrame();
}

void theGame::Update() 
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Calculating deltaTime
	float deltaSeconds			= CalculateDeltaTime();
	deltaSeconds				= (deltaSeconds > 0.2f) ? 0.2f : deltaSeconds;									// Can't go slower than 5 fps

	m_timeSinceTransitionBegan	+=	deltaSeconds;
	m_timeSinceStartOfTheGame	+=	deltaSeconds;
	
	// For UBOTesting..
	g_theRenderer->UpdateTime( deltaSeconds, deltaSeconds );

	// If in transition to another gameState
	if( m_nextGameState != NONE )
	{
		// Update the transition effects' alpha ( FADE OUT )
		float fadeOutFraction	= ClampFloat01( (float)m_timeSinceTransitionBegan / m_halfTransitionTime );		// Goes from 0 to 1
		m_fadeEffectAlpha		= fadeOutFraction;																// Goes from 0 to 1

		// If time to end fade out..
		if( m_timeSinceTransitionBegan >= m_halfTransitionTime )
			ConfirmTransitionToNextState();

		return;
	}

	// If just came in to this gameState..
	if( m_timeSinceTransitionBegan < m_halfTransitionTime )
	{
		// Update the transition effects' alpha ( FADE IN )
		float fadeOutFraction	= ClampFloat01( (float)m_timeSinceTransitionBegan / m_halfTransitionTime );		// Goes from 0 to 1
		m_fadeEffectAlpha		= 1.f - fadeOutFraction;														// Goes from 1 to 0
	}
	else																										// When m_timeSinceTransitionBegan is very big & > m_halfTransitionTime (can happen because of a breakpoint in next gameState)
		m_fadeEffectAlpha		= 0.f;		

	// Continue updating the currentGameState
	switch (m_currentGameState)
	{
	case ATTRACT:
		Update_Attract( deltaSeconds );
		break;
	case MENU:
		Update_Menu( deltaSeconds );
		break;
	case LEVEL:
		Update_Level( deltaSeconds );
		break;
	default:
		ERROR_AND_DIE( "Error: No valid gamestate found..! | theGame::Update()" );
		break;
	}
}

void theGame::Render() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	switch (m_currentGameState)
	{
	case ATTRACT:
		Render_Attract();
		break;
	case MENU:
		Render_Menu();
		break;
	case LEVEL:
		Render_Level();
		break;
	default:
		ERROR_AND_DIE( "Error: No valid gamestate found..! | theGame::Render()" );
		break;
	}

	// Render the Transition Effects - according to alpha set by Update()
	Rgba overlayBoxColor;
	overlayBoxColor.SetAsFloats( 0.f, 0.f, 0.f, m_fadeEffectAlpha );

	g_theRenderer->BindCamera( m_gameCamera );

	g_theRenderer->UseShader( nullptr );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawAABB( m_default_screen_bounds, overlayBoxColor );
}

void theGame::StartTransitionToState( GameStates nextGameState )
{
	m_nextGameState	= nextGameState;

	// Reset appropriate variables for the transitionEffect..
	m_timeSinceTransitionBegan = 0;
}

void theGame::ConfirmTransitionToNextState()
{
	m_currentGameState	= m_nextGameState;
	m_nextGameState		= NONE;

	m_timeSinceTransitionBegan = 0;
}

void theGame::Update_Attract( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Menu handles the state transition
	m_attractMenu->Update( deltaSeconds );
}

void theGame::Render_Attract() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Title
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	
	g_theRenderer->DrawTextInBox2D( "Mekorama", Vector2(0.5f, 0.6f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "( Use Keyboard )", Vector2(0.5f, 0.02f), m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );

	// Menu
	m_attractMenu->Render();
}

void theGame::Update_Menu( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	UNUSED( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		StartTransitionToState( ATTRACT );

	m_levelSelectionMenu->Update( deltaSeconds );
}

void theGame::Render_Menu() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "Jump to..", Vector2(0.5f, 0.6f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "(Press ~ for DevConsole )", Vector2(0.5f, 0.02f), m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	

	m_levelSelectionMenu->Render();
}

void theGame::Update_Level( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		StartTransitionToState( MENU );

	m_currentLevel->Update( deltaSeconds );
}

void theGame::Render_Level() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	m_currentLevel->Render();
}

void theGame::GoToMenuState( char const * actionName )
{
	UNUSED( actionName );
	StartTransitionToState( MENU );
}

void theGame::QuitGame( char const * actionName )
{
	UNUSED( actionName );
	g_theApp->m_isQuitting = true;
}

void theGame::LevelSelected( char const *actionName )
{
	if( m_currentLevel != nullptr )
		delete m_currentLevel;

	m_currentLevel = new Level( actionName, *m_playerRobot );
	m_currentLevel->Startup();

	StartTransitionToState( LEVEL );
}

void theGame::RenderLoadingScreen() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "Loading..", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

float theGame::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - m_lastFramesTime);
	m_lastFramesTime = currentTime;

	return deltaSeconds;
}

double theGame::GetTimeSinceGameStarted() const
{
	return m_timeSinceStartOfTheGame;
}