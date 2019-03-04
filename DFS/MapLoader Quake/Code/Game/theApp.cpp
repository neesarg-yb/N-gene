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

theApp::theApp()
{
	g_theRenderer = new Renderer();
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

	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void theApp::Startup()
{
	CommandRegister( "profiler", ShowHideProfileConsole );

	DebugRendererStartup( g_theRenderer );

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
		DevConsole::GetInstance()->Render();
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
