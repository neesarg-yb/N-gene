#include "theApp.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Audio/AudioSystem.hpp"

void QuitTheApp( Command& cmd );

theApp::theApp()
{
	g_theRenderer		= new Renderer();
	g_theGame			= new theGame();
	g_theInput			= new InputSystem();
	g_theAudioSystem	= new AudioSystem();

	DevConsole::InitalizeSingleton( *g_theRenderer );
	Window::GetInstance()->AddMessageHandler( DevConsole::ConsoleMessageHandler );
}

theApp::~theApp()
{
	DevConsole::DestroySingleton();

	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theGame;
	g_theGame = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void theApp::Startup()
{
	g_theGame->Startup();
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
	// Profiler MarkFrame
	Profiler::GetInstance()->MarkFrame();

	g_theAudioSystem->BeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theGame->BeginFrame();
}

void theApp::EndFrame() {
	g_theGame->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
	g_theAudioSystem->EndFrame();
}

void theApp::Update() {
	g_theGame->Update();

	if( DevConsole::GetInstance()->IsOpen() )
		DevConsole::GetInstance()->Update( *g_theInput );
}

void theApp::Render() {
	g_theGame->Render();

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