#include "Engine/Input/InputSystem.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

void RunMessagePump() {			// NOTE: standalone function in InputSystem.cpp (not an InputSystem method)

	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" function
	}
}

InputSystem::InputSystem()
{	
	for(int i=0; i<4; i++) {
		m_controller[i].m_controllerNumber = i;
	}
}

InputSystem::~InputSystem()
{

}

void InputSystem::BeginFrame() {
	UpdateController();
	UpdateKeyboard();
	UpdateMouse();
	RunMessagePump();		// Ask Windows to call our registred WinProc function with WM_KEYDOWN notifications, etc.
}

void InputSystem::EndFrame() {

}

void InputSystem::UpdateMouse()
{
// 	// Absolute Mode - I get mouse position and I can potentially lock to the screen
// 	m_mousePositionLastFrame = m_mousePositionThisFrame;
// 	m_mousePositionThisFrame = GetMouseClientPosition();
// 
// 	// Relative Mode - I care about deltas; I reset to the center ( meaning, mutually exclusive modes )
// 	if( m_mouseMode = MOUSEMODE_RELATIVE )
// 	{
// 		m_mousePositionLastFrame = GetCenterOfClientWindow();
// 		SetMouseScreenPosition( m_mousePositionLastFrame );
// 	}
}

void InputSystem::UpdateKeyboard() {
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. message
	for( int keyCode = 0; keyCode < InputSystem::NUM_KEYS; keyCode++) {
		m_keyStates[ keyCode ].keyJustPressed = false;
		m_keyStates[ keyCode ].keyJustReleasesd = false;
	}

}

void InputSystem::UpdateController() {
	for(int i=0; i<4; i++) {
		m_controller[i].UpdateControllerState();
	}
}

void InputSystem::OnKeyPressed( unsigned char keyCode ) {
	if(m_keyStates[ keyCode ].keyIsDown != true) {			// Update keyJustPressed
		m_keyStates[ keyCode ].keyJustPressed = true;
	}
	m_keyStates[ keyCode ].keyIsDown = true;				// Update keyIsDown
	
}

void InputSystem::OnKeyReleased( unsigned char keyCode ) {
	if(m_keyStates[ keyCode ].keyIsDown == true) {			// Update keyJustReleased
		m_keyStates[ keyCode ].keyJustReleasesd = true;
	}
	m_keyStates[ keyCode ].keyIsDown = false;				// Update keyIsDown
	
}

bool InputSystem::IsKeyPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].keyIsDown;
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].keyJustPressed;
}

bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].keyJustReleasesd;
}