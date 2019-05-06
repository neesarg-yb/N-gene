#include "theGame.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Game/theApp.hpp"
#include "Game/Game States/Attract.hpp"
#include "Game/Game States/LevelSelect.hpp"
#include "Game/Game States/MinecraftWorld.hpp"

void EchoTestCommand( Command& cmd )
{
	ConsolePrintf( "%s", cmd.GetNextString().c_str() );
}

theGame::theGame()
{
	// Set global variable
	g_theGame = this;

	// Initialize the game clock
	g_gameClock = new Clock( GetMasterClock() );

	m_lastFramesTime = GetCurrentTimeSeconds();

	// Fonts for loading screen
	m_textBmpFont = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	// Camera Setup
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_gameCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );										// To set NDC styled ortho


	// TESTING : NAMED PROPERTIES
	std::string lastName( "Eiserloh" );

	NamedProperties p;
	p.Set( "FirstName", "Squirrel" ); 	// Setting as c-string (const char*) data...
	p.Set( "LastName", lastName );	// Setting as std::string data...
	p.Set( "Height", 1.93f );
	p.Set( "Age", 45 );
	p.Set( "IsMarried", true );
	p.Set( "Position", Vector2( 3.5f, 6.2f ) );
	p.Set( "EyeColor", Rgba( 77, 38, 23 ) );
	p.Set( "Height", 2U );

	float height = p.Get( "Height", 0.f );
	std::string fname = p.Get( "FirstName1", "Neesarg" );

	UNUSED( height );
	UNUSED( fname );

	// TESTING : EVENT SYSTEM
	FireEvent( "Sunrise" );
	SubscribeEventCallbackFunction( "Sunrise", MyTestEventStaticFunction );
	FireEvent( "Sunrise" );
	UnsubscribeEventCallbackFunction( "Sunrise", MyTestEventStaticFunction );
	FireEvent( "Sunrise" );
}

theGame::~theGame()
{
	while ( m_gameStates.size() > 0 )
	{
		delete m_gameStates.back();
		m_gameStates.pop_back();
	}

	delete g_defaultMaterial;
	g_defaultMaterial = nullptr;

	delete m_textBmpFont;
}

void theGame::Startup()
{
	// Render Loading Screen
	g_theRenderer->BeginFrame();
	RenderLoadingScreen();
	g_theRenderer->EndFrame();
	
	// Default Material
	g_defaultMaterial = Material::CreateNewFromFile( "Data\\Materials\\default.material" );

	// Console stuffs
	CommandRegister( "echo", EchoTestCommand );
	ConsolePrintf( RGBA_GREEN_COLOR, "%i Hello World!", 1 );

	// Setup the game states
	GameState* attractGS = new Attract( g_gameClock );
	AddNewGameState( attractGS );

	GameState* levelSelectGS = new LevelSelect( g_gameClock );
	AddNewGameState( levelSelectGS );

	GameState* collisionAvoidanceScene = new MinecraftWorld( g_gameClock, "World 1" );	// If you change sceneName, change it in LevelSelect::LevelSelected(), too!
	AddNewGameState( collisionAvoidanceScene );

	// Set game state to begin with
	SetCurrentGameState( attractGS->m_name );
}

void theGame::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	DebugRendererBeginFrame( GetMasterClock() );

	m_currentGameState->BeginFrame();
}

void theGame::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	m_currentGameState->EndFrame();
}

void theGame::Update() 
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Calculating deltaTime
	float deltaSeconds			= (float) g_gameClock->GetFrameDeltaSeconds();
	deltaSeconds				= (deltaSeconds > 0.2f) ? 0.2f : deltaSeconds;									// Can't go slower than 5 fps

	m_timeSinceTransitionBegan	+=	deltaSeconds;
	m_timeSinceStartOfTheGame	+=	deltaSeconds;
	
	// For UBOTesting..
	g_theRenderer->UpdateTime( deltaSeconds, deltaSeconds );

	// If in transition to another gameState
	if( m_nextGameStateName != "NONE" )
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
	m_currentGameState->Update();

	DebugRender2DText( 0.f, Vector2( 700.f, 450.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, Stringf("FPS %3.d", (int)(1.f / deltaSeconds)) );
}

void theGame::Render() const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	m_currentGameState->Render( m_gameCamera );

	// Render the Transition Effects - according to alpha set by Update()
	Rgba overlayBoxColor;
	overlayBoxColor.SetAsFloats( 0.f, 0.f, 0.f, m_fadeEffectAlpha );

	g_theRenderer->BindCamera( m_gameCamera );

	g_theRenderer->UseShader( nullptr );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawAABB( m_default_screen_bounds, overlayBoxColor );

	DebugRendererLateRender( m_gameCamera );
}

bool theGame::SetCurrentGameState( std::string const &gsName )
{
	GameState *gs = nullptr;
	FindGameStateNamed( gsName, gs );
	if( gs == nullptr )
		return false;

	m_currentGameState	= gs;
	m_nextGameStateName	= "NONE";
	return true;
}

void theGame::StartTransitionToState( std::string const &stateName )
{
	m_nextGameStateName	= stateName;

	// Reset appropriate variables for the transitionEffect..
	m_timeSinceTransitionBegan = 0;
}

void theGame::ConfirmTransitionToNextState()
{
	// Set current game state
	int idx	= FindGameStateNamed( m_nextGameStateName, m_currentGameState );
	GUARANTEE_RECOVERABLE( idx >= 0, Stringf("Couldn't find gamestate named \"%s\"", m_nextGameStateName.c_str() ) );

	// Inform about this game state about transition
	m_currentGameState->JustFinishedTransition();

	// Finish the transition
	m_nextGameStateName	= "NONE";
	m_timeSinceTransitionBegan = 0;
}

void theGame::QuitGame( char const * actionName )
{
	UNUSED( actionName );
	g_theApp->m_isQuitting = true;
}

void theGame::AddNewGameState( GameState* gsToAdd )
{
	if( gsToAdd == nullptr )
		return;

	GameState* alreadyExists = nullptr;
	int idx = FindGameStateNamed( gsToAdd->m_name, alreadyExists );
	
	if( alreadyExists != nullptr )
	{
		// Replace
		delete m_gameStates[ idx ];
		m_gameStates[ idx ] = gsToAdd;
	}
	else // Add new
		m_gameStates.push_back( gsToAdd );
}

GameState* theGame::RemoveGameStateNamed( std::string const &gsName )
{
	GameState *gs = nullptr;
	int idx = FindGameStateNamed( gsName, gs );

	if( gs != nullptr )
	{
		// Fast Detach
		std::swap( m_gameStates[idx], m_gameStates.back() );
		m_gameStates.pop_back();
	}

	return gs;
}

int theGame::FindGameStateNamed( std::string const &stateName, GameState *&outGameState )
{
	for( int i = 0; i < m_gameStates.size(); i++ )
	{
		GameState* gameState = m_gameStates[i];
		if( gameState->m_name == stateName )
		{
			outGameState = gameState;
			return i;
		}
	}

	return -1;
}

void theGame::RenderLoadingScreen() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "Loading..", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

double theGame::GetTimeSinceGameStarted() const
{
	return m_timeSinceStartOfTheGame;
}

bool theGame::MyTestEventStaticFunction( NamedProperties &args )
{
	UNUSED( args );
	return false;
}
