#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/theApp.hpp"

bool gameWonFromCommand = false;

void WinTheGame( Command& cmd )
{
	UNUSED( cmd );
	gameWonFromCommand = true;
}

void EchoTestCommand( Command& cmd )
{
	ConsolePrintf( "%s", cmd.GetNextString().c_str() );
}

theGame::theGame()
{
	m_lastFramesTime = GetCurrentTimeSeconds();
	
	// Construction
	m_currentBattle = new Battle();

	// Fonts for loading screen
	m_textBmpFont = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	// Camera Setup
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_gameCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho
}

theGame::~theGame()
{
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
	CommandRegister( "win_game", WinTheGame );
	ConsolePrintf( RGBA_GREEN_COLOR, "%d Hello World!", 1 );

	// Seting up the Attract Menu
	m_attractMenu = new UIMenu( *g_theInput, *g_theRenderer, AABB2( 0.45f, 0.45f, 0.55f, 0.55f ) );

	m_attractMenu->AddNewMenuAction( MenuAction( "Quit", &quitStdFunc ) );
	m_attractMenu->AddNewMenuAction( MenuAction( "Start", &startStdFunc ) );
	m_attractMenu->m_selectionIndex = 1;

	// Load All Sounds
	m_attractMusic				= g_theAudioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	m_anticipateMusic			= g_theAudioSystem->CreateOrGetSound( "Data/Audio/Anticipation.mp3" );
	m_battleBackgroundMusic		= g_theAudioSystem->CreateOrGetSound( "Data/Audio/GameplayMusic.mp3" );

	// Audio Group
	g_theAudioSystem->LoadAudioGroupFromFile( "Data/Audio/Gun_Pistol_Shot.audiogroup" );

	// Start playing attract sound
	m_attractPlayback = g_theAudioSystem->PlaySound( m_attractMusic, true, 0.5f );

	// Call Startup for other classes
	m_currentBattle->Startup();
}

void theGame::BeginFrame()
{
	if( m_currentGameState == BATTLE )
		m_currentBattle->BeginFrame();
}

void theGame::EndFrame()
{
	if( m_currentGameState == BATTLE )
		m_currentBattle->EndFrame();
}

void theGame::Update() 
{
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
	case BATTLE:
		Update_Battle( deltaSeconds );
		break;
	case VICTORY:
		Update_Victory( deltaSeconds );
		break;
	default:
		ERROR_AND_DIE( "Error: No valid gamestate found..! | theGame::Update()" );
		break;
	}
}

void theGame::Render() const
{
	switch (m_currentGameState)
	{
	case ATTRACT:
		Render_Attract();
		break;
	case MENU:
		Render_Menu();
		break;
	case BATTLE:
		Render_Battle();
		break;
	case VICTORY:
		Render_Victory();
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

	// Audio Playback
	g_theAudioSystem->StopSound( m_attractPlayback );
	g_theAudioSystem->StopSound( m_anticipatePlayback );
	g_theAudioSystem->StopSound( m_battleBackgroundPlayback );

	switch (nextGameState)
	{
	case NONE:
		break;
	case ATTRACT:
		m_attractPlayback = g_theAudioSystem->PlaySound( m_attractMusic, true, 0.5f );
		break;
	case MENU:
		m_anticipatePlayback = g_theAudioSystem->PlaySound( m_anticipateMusic, false, 0.5f );
		break;
	case BATTLE:
		m_battleBackgroundPlayback = g_theAudioSystem->PlaySound( m_battleBackgroundMusic, true, 0.5f, 1.f );
		break;
	case NUM_GAME_STATES:
		break;
	default:
		break;
	}
}

void theGame::ConfirmTransitionToNextState()
{
	m_currentGameState	= m_nextGameState;
	m_nextGameState		= NONE;

	m_timeSinceTransitionBegan = 0;
}

void theGame::Update_Attract( float deltaSeconds )
{
	// Menu handles the state transition
	m_attractMenu->Update( deltaSeconds );
}

void theGame::Render_Attract() const
{
	// Title
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	
	g_theRenderer->DrawTextInBox2D( "TANK WAR", Vector2(0.5f, 0.6f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "( Use Controller )", Vector2(0.5f, 0.02f), m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );

	// Menu
	m_attractMenu->Render();
}

void theGame::Update_Menu( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		StartTransitionToState( ATTRACT );
	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed )
		StartTransitionToState( ATTRACT );
	if( g_theInput->WasKeyJustPressed( VK_Codes::SPACE ) )
		StartTransitionToState( BATTLE );
	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed )
		StartTransitionToState( BATTLE );
}

void theGame::Render_Menu() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "MAIN MENU\n \n Press (A) to jump to the battle.. \n \n (Press ~ for DevConsole )", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Update_Battle( float deltaSeconds )
{
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		StartTransitionToState( MENU );
	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed )
		StartTransitionToState( MENU );

	// Profiler Test
	Profiler::GetInstance()->Push( "Battle::Update" );

	m_currentBattle->Update( deltaSeconds );

	// Profiler Test
	Profiler::GetInstance()->Pop();

	if( m_currentBattle->IsBattleWon() || gameWonFromCommand )
	{
		StartTransitionToState( VICTORY );
		gameWonFromCommand = false;
	}
}

void theGame::Update_Victory( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed )
	{
		// Create a new level
		delete m_currentBattle;
		m_currentBattle = new Battle();
		m_currentBattle->Startup();

		// Start transition to that level
		StartTransitionToState( BATTLE );
	}
}

void theGame::Render_Battle() const
{
	m_currentBattle->Render();
}

void theGame::Render_Victory() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "Victory!", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "Press (A) to continue..", Vector2(0.5f, 0.02f), m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
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