#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/LogSystem/LogSystem.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Game/theApp.hpp"
#include "Game/Game States/Attract.hpp"
#include "Game/Game States/LevelSelect.hpp"
#include "Game/Game States/Level1.hpp"

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
		ConsolePrintf( RGBA_RED_COLOR, "Error: Command needs two valid argument!" );
		return;
	}

	idx	 = atoi( arg1.c_str() );
	addr = NetworkAddress( arg2.c_str() );

	NetworkSession		*session	= theGame::GetSession();
	NetworkConnection	*connection	= session->AddConnection( idx, addr );
	if( connection == nullptr )
		ConsolePrintf( RGBA_RED_COLOR, "Failed to add connection!" );
	else
		ConsolePrintf( RGBA_GREEN_COLOR, "Connection added at index [%d]", idx );
}

void SessionSendPing( Command &cmd )
{
	int idx;

	std::string arg1 = cmd.GetNextString();
	std::string arg2 = cmd.GetRemainingCommandInOneString();
	if( arg1 == "" )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Provide a valid index" );
		return;
	}
	else
		idx = atoi( arg1.c_str() );

	NetworkSession		*session  = theGame::GetSession();
	NetworkConnection	*receiver = session->GetConnection( idx );
	if( receiver == nullptr )
	{
		ConsolePrintf( RGBA_RED_COLOR, "No connection at index %d", idx );
		return;
	}

	NetworkMessage msg( "ping" );
	msg.Write( arg2 );

	receiver->Send( msg );
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
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}

	idx		= atoi( arg1.c_str() );
	value1	= (float) atof( arg2.c_str() );
	value2	= (float) atof( arg3.c_str() );

	NetworkSession		*session	= theGame::GetSession();
	NetworkConnection	*receiver	= session->GetConnection( idx );
	if( receiver == nullptr )
	{
		ConsolePrintf( RGBA_RED_COLOR, "No connection at index %d", idx );
		return;
	}

	NetworkMessage msg( "add" );
	msg.Write( value1 );
	msg.Write( value2 );
	receiver->Send( msg );
}

void NetSimLoss( Command &cmd )
{
	std::string lossFractionStr = cmd.GetNextString();

	if( lossFractionStr == "" )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}

	float fraction = (float)atof( lossFractionStr.c_str() );
	fraction = ClampFloat01( fraction );

	theGame::GetSession()->SetSimulationLoss( fraction );
	ConsolePrintf( RGBA_GREEN_COLOR, "NetSimLoss: loss fraction set to %f", fraction );
}

void NetSimLag( Command &cmd )
{
	std::string minMsStr = cmd.GetNextString();
	std::string maxMsStr = cmd.GetNextString();

	if( minMsStr == "")
	{
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}
	
	int minLag_ms = atoi( minMsStr.c_str() );
	int maxLag_ms = 0;							// Max lag defaults to zero, if not provided
	if( maxMsStr != "" )
		maxLag_ms = atoi( maxMsStr.c_str() );

	theGame::GetSession()->SetSimulationLatency( (uint)minLag_ms, (uint)maxLag_ms );
	ConsolePrintf( RGBA_GREEN_COLOR, "NetSimLag: LagRange [ %d, %d ] set!", minLag_ms, maxLag_ms );
}

void SetSessionSendRate( Command &cmd )
{
	std::string sendRateStr = cmd.GetNextString();

	if( sendRateStr == "" )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}

	int sendRate = atoi( sendRateStr.c_str() );
	if( sendRate <= 0 || sendRate > 0xff )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Provide send_rate in range (0, 255]..!" );
		return;
	}

	theGame::GetSession()->SetSimulationSendFrequency( (uint8_t)sendRate );
	ConsolePrintf( RGBA_GREEN_COLOR, "Send Rate set to %dhz", theGame::GetSession()->GetSimulatedSendFrequency() );
}

void SetConnectionSendRate( Command &cmd )
{
	std::string connectionIdxStr = cmd.GetNextString();
	std::string sendRateStr		 = cmd.GetNextString();

	if( sendRateStr == "" || connectionIdxStr == "" )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}

	int connectionIdx	= atoi( connectionIdxStr.c_str() );
	int sendRate		= atoi( sendRateStr.c_str() );
	if( sendRate <= 0 || sendRate > 0xff )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Provide send_rate in range (0, 255]..!" );
		return;
	}

	NetworkConnection *connection = theGame::GetSession()->GetConnection( connectionIdx );
	if( connection == nullptr )
	{
		ConsolePrintf( RGBA_YELLOW_COLOR, "Connection at [%d] doesn't exists!", connectionIdx  );
		return;
	}

	connection->SetSendFrequencyTo( (uint8_t)sendRate );
	ConsolePrintf( RGBA_GREEN_COLOR, "Send Rate of connection [%d] set to %dhz", connectionIdx, connection->GetCurrentSendFrequency() );
}

void SetHeartbeatRate( Command &cmd )
{
	std::string rateInHzStr = cmd.GetNextString();

	if( rateInHzStr == "" )
	{
		ConsolePrintf( RGBA_RED_COLOR, "Not all arguments are provided.." );
		return;
	}

	float rateHz = (float)atof( rateInHzStr.c_str() );
	bool success = theGame::GetSession()->SetHeartbeatFrequency( rateHz );

	if( success )
		ConsolePrintf( RGBA_GREEN_COLOR, "Heartbeat rate set to %.4fhz", theGame::GetSession()->GetHeartbeatFrequency() );
	else
		ConsolePrintf( RGBA_YELLOW_COLOR, "Can't set that heartrate frequency..!" );
}

bool OnAddResponse( NetworkMessage const &msg, NetworkSender &from )
{
	UNUSED( from );

	float val1 = 0.f;
	float val2 = 0.f;
	float sum  = 0.f;

	msg.Read( val1 );
	msg.Read( val2 );
	msg.Read( sum );

	ConsolePrintf( "AddResponse: %f + %f = %f", val1, val2, sum );

	return true;
}

bool OnPing( NetworkMessage const &msg, NetworkSender &from )
{
	char str[256];
	msg.Read( str, 256 );

	ConsolePrintf( "Received ping from %s => %s", from.address.AddressToString().c_str(), str ); 

	// ping responds with pong
	NetworkMessage pong( "pong" ); 
	if( from.connection != nullptr )
		from.connection->Send( pong );
	else
		from.session.SendDirectMessageTo( pong, from.address );

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnPong( NetworkMessage const &msg, NetworkSender &from )
{
	UNUSED( msg );

	ConsolePrintf( "PONG! Received from %s", from.address.AddressToString().c_str() );
	return false;
}

bool OnAdd( NetworkMessage const &msg, NetworkSender &from )
{
	float val0 = 0;
	float val1 = 0;
	float sum;

	if( !msg.Read( val0 ) || !msg.Read( val1 ) )
		return false;

	sum = val0 + val1;
	std::string additionResponse = Stringf( "Add: %f + %f = %f", val0, val1, sum );
	ConsolePrintf( additionResponse.c_str() );

	// Send back a response here, if you want..
	NetworkMessage replyMsg( "add_response" );
	replyMsg.Write( val0 );
	replyMsg.Write( val1 );
	replyMsg.Write( sum );
	if( from.connection != nullptr )
		from.connection->Send( replyMsg );
	else
		from.session.SendDirectMessageTo( replyMsg, from.address );

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
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );										// To set NDC styled ortho
}

theGame::~theGame()
{
	delete m_session;

	while ( m_gameStates.size() > 0 )
	{
		delete m_gameStates.back();
		m_gameStates.pop_back();
	}

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
	CommandRegister( "add_connection", AddSessionConnection );
	CommandRegister( "send_ping", SessionSendPing );
	CommandRegister( "send_add", SessionSendAdd );
	CommandRegister( "net_sim_loss", NetSimLoss );
	CommandRegister( "net_sim_lag", NetSimLag );
	CommandRegister( "net_set_session_send_rate", SetSessionSendRate );
	CommandRegister( "net_set_connection_send_rate", SetConnectionSendRate );
	CommandRegister( "net_set_heart_rate", SetHeartbeatRate );
	ConsolePrintf( RGBA_GREEN_COLOR, "%i Hello World!", 1 );

	// Setup the game states
	GameState* attractGS = new Attract();
	AddNewGameState( attractGS );

	GameState* levelSelectGS = new LevelSelect();
	AddNewGameState( levelSelectGS );

	GameState* quaternionsTest = new Level1();
	AddNewGameState( quaternionsTest );

	// Set game state to begin with
	SetCurrentGameState( attractGS->m_name );

	// Network Session
	m_session = new NetworkSession( g_theRenderer );
	m_session->RegisterNetworkMessage( "ping", OnPing );
	m_session->RegisterNetworkMessage( "pong", OnPong );
	m_session->RegisterNetworkMessage( "add",  OnAdd );
	m_session->RegisterNetworkMessage( "add_response",  OnAddResponse );

	// For now we'll just shortcut to being a HOST
	// "bound" state
	// This creates the socket(s) we can communicate on..
	bool bindSuccess = m_session->BindPort( GAME_PORT, 1U );
	ConsolePrintf( RGBA_KHAKI_COLOR, "Network Session Bind Succes = %d; Address = %s", (int)bindSuccess, m_session->m_mySocket->m_address.AddressToString().c_str() );
	m_session->m_mySocket->EnableNonBlocking();
}

void theGame::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Network Session
	m_session->ProcessIncoming();

	m_currentGameState->BeginFrame();
}

void theGame::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	m_currentGameState->EndFrame();

	// Network Session
	m_session->ProcessOutgoing();
}

void theGame::Update() 
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Calculating deltaTime
	float deltaSeconds			= (float) GetMasterClock()->GetFrameDeltaSeconds();
	deltaSeconds				= (deltaSeconds > 0.2f) ? 0.2f : deltaSeconds;									// Can't go slower than 5 fps

	m_timeSinceTransitionBegan	+=	deltaSeconds;
	m_timeSinceStartOfTheGame	+=	deltaSeconds;

	// Remote Command Service
	g_rcs->Update( deltaSeconds );
	
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
	m_currentGameState->Update( deltaSeconds );
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

	// Network Session UI
	m_session->Render();
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
	int idx	= FindGameStateNamed( m_nextGameStateName, m_currentGameState );
	GUARANTEE_RECOVERABLE( idx >= 0, Stringf("Couldn't find gamestate named \"%s\"", m_nextGameStateName.c_str() ) );

	m_nextGameStateName	= "NONE";
	m_timeSinceTransitionBegan = 0;
}

void theGame::QuitGame( char const * actionName )
{
	UNUSED( actionName );
	g_theApp->m_isQuitting = true;
}

NetworkSession* theGame::GetSession()
{
	return g_theGame->m_session;
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