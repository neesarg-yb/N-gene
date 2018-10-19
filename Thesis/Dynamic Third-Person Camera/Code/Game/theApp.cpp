#include "theApp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerConsole.hpp"
#include "Engine/LogSystem/LogSystem.hpp"

void QuitTheApp( Command& cmd );

void CreateNewAppInstace()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	wchar_t buffer[MAX_PATH]; 
	GetModuleFileName(NULL, buffer, MAX_PATH) ;
	CreateProcess(buffer, 0, 0, FALSE, 0, 0, 0, 0, &si, &pi);
}

void CloneMyself( Command& cmd )
{
	std::string number = cmd.GetNextString();
	int spawnCount = 1;
	if( number != "" )
		spawnCount = ::atoi( number.c_str() );

	// Just for safety
	if( spawnCount > 10 )
		spawnCount = 10;

	for( int i = 0; i < spawnCount; i++ )
		CreateNewAppInstace();
}

void ShowHideProfileConsole( Command& cmd )
{
	UNUSED( cmd );
	if( ProfileConsole::GetInstance()->IsOpen() )
	{
		ProfileConsole::GetInstance()->Close();
		DevConsole::GetInstance()->Close();
	}
	else
	{
		ProfileConsole::GetInstance()->Open();
		DevConsole::GetInstance()->Close();
	}
}

void SendEchoToClient( Command &cmd )
{
	std::string firstArg	= cmd.GetNextString();
	std::string secondArg	= cmd.GetRemainingCommandInOneString();

	// If first arg is idx=n, send message to nth connection..
	if( firstArg.length() > 4 && firstArg.at(0) == 'i' && firstArg.at(1) == 'd' && firstArg.at(2) == 'x' && firstArg.at(3) == '=' )
	{
		std::string idStr	= std::string( firstArg, 4 );
		uint		idx		= (uint) atoi( idStr.c_str() );
		g_rcs->SendMessageToConnection( idx, true, secondArg.c_str() );
	}
	else
	{
		// Send whole message to 0th connection
		std::string fullMessage = firstArg;
		if( secondArg != "" )
			fullMessage += ( " " + secondArg );

		g_rcs->SendMessageToConnection( 0U, true, fullMessage.c_str() );
	}
}

void SendCommand( Command &cmd )
{
	std::string firstArg	= cmd.GetNextString();
	std::string secondArg	= cmd.GetRemainingCommandInOneString();

	// If first arg is idx=n, send message to nth connection..
	if( firstArg.length() > 4 && firstArg.at(0) == 'i' && firstArg.at(1) == 'd' && firstArg.at(2) == 'x' && firstArg.at(3) == '=' )
	{
		std::string idStr	= std::string( firstArg, 4 );
		uint		idx		= (uint) atoi( idStr.c_str() );
		g_rcs->SendMessageToConnection( idx, false, secondArg.c_str() );
	}
	else
	{
		// Send whole message to 0th connection
		std::string fullMessage = firstArg;
		if( secondArg != "" )
			fullMessage += ( " " + secondArg );

		g_rcs->SendMessageToConnection( 0U, false, fullMessage.c_str() );
	}
}

void SendCommandToAll( Command &cmd )
{
	std::string message = cmd.GetRemainingCommandInOneString();
	g_rcs->SendMessageToAllConnections( false, message.c_str(), true );
}

void BroadcastCommand( Command &cmd )
{
	std::string message = cmd.GetRemainingCommandInOneString();
	g_rcs->SendMessageToAllConnections( false, message.c_str(), false );
}

void HostAtPort( Command &cmd )
{
	std::string portStr = cmd.GetNextString();
	if( portStr == "" )
		g_rcs->HostAtPort();		// Host to default predefined port!
	else
	{
		int port = atoi( portStr.c_str() );
		g_rcs->HostAtPort( (uint16_t) port );
	}
}

void ConnectToHost( Command &cmd )
{
	std::string hostAddressStr = cmd.GetNextString();
	if( hostAddressStr == "" )
		return;

	g_rcs->ConnectToNewHost( hostAddressStr.c_str() );
}

void RCSSetEcho( Command &cmd )
{
	std::string echoBoolStr = cmd.GetNextString();
	bool		echoBool = true;

	if( echoBoolStr != "" )
		SetFromText( echoBool, echoBoolStr.c_str() );

	ConsolePrintf( "RCS: %s", !echoBool ? "Echo turned OFF.." : "Echo turned ON.." );

	g_rcs->IgnoreEcho( !echoBool );
}

theApp::theApp()
{
	g_theRenderer = new Renderer();
	g_rcs = new RemoteCommandService( g_theRenderer );
	g_theGame = new theGame();
	g_theInput = new InputSystem();

	DevConsole::InitalizeSingleton( *g_theRenderer );
	Window::GetInstance()->AddMessageHandler( DevConsole::ConsoleMessageHandler );
}

theApp::~theApp()
{
	DevConsole::DestroySingleton();

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theGame;
	g_theGame = nullptr;

	delete g_rcs;
	g_rcs = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void theApp::Startup()
{
	CommandRegister( "profiler", ShowHideProfileConsole );
	CommandRegister( "re", SendEchoToClient );
	CommandRegister( "rc", SendCommand );
	CommandRegister( "rca", SendCommandToAll );
	CommandRegister( "rcb", BroadcastCommand );
	CommandRegister( "rc_host", HostAtPort );
	CommandRegister( "rc_join", ConnectToHost );
	CommandRegister( "rc_echo", RCSSetEcho );
	CommandRegister( "clone_process", CloneMyself );

	DebugRendererStartup( g_theRenderer, nullptr );

	g_theGame->Startup();

	DebuggerPrintf( "theApp::Startup()" );
}

void theApp::Shutdown()
{
	DebugRendererShutdown();
}

void theApp::RunFrame() {
	BeginFrame();		// [ for ENGINE components, generally ]
	Update();			// [ for  GAME  components, generally ]
	Render();			// [ for  GAME  components, generally ]
	EndFrame();			// [ for ENGINE components, generally ]

	// To Reduce CPU usage
	Sleep(1);		// or SwitchToThread();
}

void theApp::BeginFrame() {
	// Inform the Clock about frame
	TickMasterClock();

	// Profiler MarkFrame
	Profiler::GetInstance()->MarkFrame();

	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theGame->BeginFrame();
}

void theApp::EndFrame() {
	g_theGame->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

void theApp::Update() {
	g_theGame->Update();

	ProfileConsole::GetInstance()->Update( *g_theInput );

	if( DevConsole::GetInstance()->IsOpen() )
		DevConsole::GetInstance()->Update( *g_theInput );
}

void theApp::Render() {
	g_theGame->Render();

	ProfileConsole::GetInstance()->Render();

	if( DevConsole::GetInstance()->IsOpen() )
	{
		DevConsole::GetInstance()->Render();
		g_rcs->Render();
	}
}

bool theApp::IsQuitting() {
	return m_isQuitting;
}

void theApp::HandleKeyDown( unsigned char KEY_CODE )
{
	if( KEY_CODE == 192 )	// Tilde's KEY_CODE
	{
		DevConsole::GetInstance()->IsOpen() ? DevConsole::GetInstance()->Close() : DevConsole::GetInstance()->Open(); 
		return;
	}
	
	g_theInput->OnKeyPressed(KEY_CODE);
}

void theApp::HandleKeyUp( unsigned char KEY_CODE ) 
{
	if( KEY_CODE == 192 )	// Tilde's KEY_CODE
		return;					// Totally ignore it
	
	g_theInput->OnKeyReleased(KEY_CODE);
}

void theApp::HandleMouseButtonDown( eMouseButtons buttonCode )
{
	g_theInput->OnMouseButtonPressed( buttonCode );
}

void theApp::HandleMouseButtonUp( eMouseButtons buttonCode )
{
	g_theInput->OnMouseButtonReleased( buttonCode );
}
