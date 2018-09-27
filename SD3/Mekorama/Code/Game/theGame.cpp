#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"
#include "Game/theApp.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"
#include "Game/World/LevelDefinition.hpp"

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

void AddSessionConnection( Command &cmd )
{
	int				idx;
	NetworkAddress	addr;

	std::string arg1 = cmd.GetNextString();
	std::string arg2 = cmd.GetNextString();
	if( arg1 == "" || arg2 == "" )
	{
		ConsolePrintf( "Error: Command needs two valid argument!" );
		return;
	}

	idx	 = atoi( arg1.c_str() );
	addr = NetworkAddress( arg2.c_str() );

	NetworkSession		*session	= theGame::GetSession();
	NetworkConnection	*connection	= session->AddConnection( idx, addr );
	if( connection == nullptr )
		ConsolePrintf( "Failed to add connection!" );
	else
		ConsolePrintf( "Connection added at index [%d]", idx );
}

void SessionSendPing( Command &cmd )
{
	int idx;

	std::string arg1 = cmd.GetNextString();
	std::string arg2 = cmd.GetRemainingCommandInOneString();
	if( arg1 == "" )
	{
		ConsolePrintf( "Provide a valid index" );
		return;
	}
	else
		idx = atoi( arg1.c_str() );

	NetworkSession		*session  = theGame::GetSession();
	NetworkConnection	*receiver = session->GetConnection( idx );
	if( receiver == nullptr )
	{
		ConsolePrintf( "No connection at index %d", idx );
		return;
	}
	
//	NetworkMessage msg( "ping" );
//	msg.Write( arg2 );

	// receiver->Send( msg );
}

void SessionSendAdd( Command &cmd )
{
	int		idx;
	float	value1;
	float	value2;

	std::string arg1 = cmd.GetNextString();
	std::string arg2 = cmd.GetNextString();
	std::string arg3 = cmd.GetNextString();
	if( arg1 == "" || arg2 == "" || arg3 == "" )
	{
		ConsolePrintf( "Not all arguments are provided.." );
		return;
	}

	idx		= atoi( arg1.c_str() );
	value1	= (float) atof( arg2.c_str() );
	value2	= (float) atof( arg3.c_str() );

	NetworkSession		*session	= theGame::GetSession();
	NetworkConnection	*receiver	= session->GetConnection( idx );
	if( receiver == nullptr )
	{
		ConsolePrintf( "No connection at index %d", idx );
		return;
	}

//	NetworkMessage msg( "add" );
//	msg.Write( value1 );
//	msg.Write( value2 );
//	receiver->Send( msg );
}

bool OnPing( NetworkMessage const &msg, NetworkConnection &from )
{
	std::string str; 
//	msg.Read( str ); 

//	ConsolePrintf( "Received ping from %s: %s", from.connection.AddressToString().c_str(), str.c_str() ); 

	// ping responds with pong
//	NetworkMessage pong( "pong" ); 
//	from.Send( pong ); 

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnPong( NetworkMessage const &msg, NetworkConnection &from )
{
	UNUSED( msg );

//	ConsolePrintf( "PONG! Received from %s", from.connection.AddressToString().c_str() );
	return false;
}

bool OnAdd( NetworkMessage const &msg, NetworkConnection &from )
{
	float val0 = 0;
	float val1 = 0;
	float sum;

//	if( !msg.Read( val0 ) || msg.Read( val1 ) )
//		return false;

	sum = val0 + val1;
	ConsolePrintf( "Add: %f + %f = %f", val0, val1, sum );

	// Send back a response here, if you want..
	UNUSED( from );

	return true;
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

	// Network Session
	m_session = new NetworkSession();
//	m_session->RegisterNetworkMessage( "ping", OnPing );
//	m_session->RegisterNetworkMessage( "pong", OnPong );
//	m_session->RegisterNetworkMessage( "add",  OnAdd );

	// For now we'll just shortcut to being a HOST
	// "bound" state
	// This creates the socket(s) we can communicate on..
	m_session->BindPort( GAME_PORT, 1U );
}

theGame::~theGame()
{
	delete m_session;

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
	CommandRegister( "netexp_addConnection", AddSessionConnection );
	CommandRegister( "netexp_sendPing", SessionSendPing );
	CommandRegister( "netexp_sendAdd", SessionSendAdd );
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

	// Remote Command Service
	g_rcs->Update( deltaSeconds );

	// Network Session
	m_session->ProcessIncoming();

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

	// Network Session
	m_session->ProcessOutgoing();
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

NetworkSession* theGame::GetSession()
{
	return g_theGame->m_session;
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