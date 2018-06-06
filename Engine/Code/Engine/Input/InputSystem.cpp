#include "Engine/Input/InputSystem.hpp"

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
	RunMessagePump();		// Ask Windows to call our registred WinProc function with WM_KEYDOWN notifications, etc.
}

void InputSystem::EndFrame() {

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