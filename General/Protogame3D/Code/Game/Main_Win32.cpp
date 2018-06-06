#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "theApp.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"


theApp* g_theApp = nullptr;
void QuitTheApp( Command& cmd );



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
HWND g_hWnd = nullptr;							// ...becomes WindowContext::m_windowHandle
const char* APP_NAME = "Protogame3D";			// ...becomes ???


//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
// #SD1ToDo: We will move this function to a more appropriate place when we're ready
//
bool AppMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam )
{
	UNUSED( lParam );

	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		g_theApp->m_isQuitting = true;
		return false; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char) wParam;
		g_theApp->HandleKeyDown(asKey);

		// If ESC, Quit the app
		if(asKey == VK_ESCAPE) {
			// g_theApp->m_isQuitting = true;
			// return false;
		}

		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char) wParam;
		g_theApp->HandleKeyUp(asKey);

		break;
	}

	}

	return true; 
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle, float clientAspect )
{
	UNUSED( applicationInstanceHandle );

	Window *window = Window::CreateInstance( APP_NAME, clientAspect ); 
	window->AddMessageHandler( AppMessageHandler ); 
	g_hWnd = (HWND) window->GetHandle();
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	g_theApp->RunFrame();
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	g_gameConfigBlackboard = new Blackboard();

	CreateOpenGLWindow( applicationInstanceHandle, g_aspectRatio );
	Renderer::RendererStartup( g_hWnd );
	g_theApp = new theApp();		// Creating theApp class instance

	CommandRegister( "quit", QuitTheApp );
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;
	g_theApp = nullptr;

	delete g_gameConfigBlackboard;
	g_gameConfigBlackboard = nullptr;

	Renderer::RendererShutdown();
	Renderer::GLShutdown();
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() ) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}

void QuitTheApp( Command& cmd )
{
	UNUSED( cmd )
	g_theApp->m_isQuitting = true;
}