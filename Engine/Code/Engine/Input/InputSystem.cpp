#pragma once
#include "Engine/Core/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Internal/WindowsCommon.hpp"

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

void InputSystem::BeginFrame() 
{
	UpdateController();
	UpdateKeyboard();
	UpdateMouse();
	RunMessagePump();		// Ask Windows to call our registered WinProc function with WM_KEYDOWN notifications, etc.
}

void InputSystem::EndFrame() 
{

}

void InputSystem::GetStringFromClipboard( std::string &outStr )
{
	// Make sure that Windows Clipboard is opened
	GUARANTEE_RECOVERABLE( 0 != OpenClipboard( NULL ), "Error: InputSystem failed to open the clipboard!" );

	// Get the clipboard data
	HANDLE clipboardData = GetClipboardData( CF_TEXT );
	if( clipboardData == NULL )								// If there's nothing, return
		return;

	outStr = (const char *) clipboardData;

	CloseClipboard();
}

void InputSystem::UpdateMouse()
{
	// Clear all just-changed flags, in preparation for the next round of WM_*BUTTONDOWN, etc. message
	for( int buttonCode = 0; buttonCode < NUM_MOUSE_BUTTONS; buttonCode++) {
		m_mouseButtonStates[ buttonCode ].keyJustPressed = false;
		m_mouseButtonStates[ buttonCode ].keyJustReleasesd = false;
	}

	// Absolute Mode - I get mouse position and I can potentially lock to the screen
	m_mousePositionLastFrame = m_mousePositionThisFrame;
	m_mousePositionThisFrame = GetMouseClientPosition();

	// Relative Mode - I care about deltas; I reset to the center ( meaning, mutually exclusive modes )
	if( m_mouseMode == MOUSE_MODE_RELATIVE )
	{
		m_mousePositionLastFrame = GetCenterOfClientWindow();
		SetMouseClientPosition( m_mousePositionLastFrame );
	}
}

void InputSystem::UpdateKeyboard() {
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. message
	for( int keyCode = 0; keyCode < InputSystem::NUM_KEYBOARD_KEYS; keyCode++) {
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

void InputSystem::OnMouseButtonPressed( eMouseButtons buttonCode )
{
	if( m_mouseButtonStates[ buttonCode ].keyIsDown != true ) {			// Update keyJustPressed
		m_mouseButtonStates[ buttonCode ].keyJustPressed = true;
	}
	m_mouseButtonStates[ buttonCode ].keyIsDown = true;				// Update keyIsDown
}

void InputSystem::OnMouseButtonReleased( eMouseButtons buttonCode )
{
	if( m_mouseButtonStates[ buttonCode ].keyIsDown == true ) {			// Update keyJustReleased
		m_mouseButtonStates[ buttonCode ].keyJustReleasesd = true;
	}
	m_mouseButtonStates[ buttonCode ].keyIsDown = false;				// Update keyIsDown
}

bool InputSystem::IsMouseButtonPressed( eMouseButtons buttonToCheck ) const
{
	return m_mouseButtonStates[ buttonToCheck ].keyIsDown;
}

bool InputSystem::WasMousButtonJustPressed( eMouseButtons buttonToCheck ) const
{
	return m_mouseButtonStates[ buttonToCheck ].keyJustPressed;
}

bool InputSystem::WasMouseButtonJustReleased( eMouseButtons buttonToCheck ) const
{
	return m_mouseButtonStates[ buttonToCheck ].keyJustReleasesd;
}

Vector2 InputSystem::GetMouseDelta()
{
	return (m_mousePositionThisFrame - m_mousePositionLastFrame);
}

Vector2 InputSystem::GetMouseClientPosition()
{
	POINT desktopPos;
	::GetCursorPos( &desktopPos );

	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
	
	::ScreenToClient( hwnd, &desktopPos );
	POINT clientPos = desktopPos;

	return Vector2( (float)clientPos.x, (float)clientPos.y );
}

Vector2 InputSystem::GetCenterOfClientWindow()
{
	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();

	RECT clientRect;
	::GetClientRect( hwnd, &clientRect );		// This should give you ( 0, 0, widthOfClient, heightOfClient )

	Vector2 clientCenter;
	clientCenter.x		 = ( clientRect.left + clientRect.right ) * 0.5f;
	clientCenter.y		 = ( clientRect.top + clientRect.bottom ) * 0.5f;

	return clientCenter;
}

void InputSystem::SetMouseModeTo( eMouseModes mouseMode )
{
	switch (mouseMode)
	{
	case MOUSE_MODE_ABSOLUTE:
		m_mouseMode = MOUSE_MODE_ABSOLUTE;
		break;
	case MOUSE_MODE_RELATIVE:
		SetMouseClientPosition( GetCenterOfClientWindow() );
		m_mouseMode = MOUSE_MODE_RELATIVE;
		break;
	default:
		GUARANTEE_RECOVERABLE( false, "Warning: mouseMode is invalid!" );
		break;
	}
}

void InputSystem::SetMouseScreenPosition( Vector2 desktopPosition )
{
	::SetCursorPos( (int)desktopPosition.x, (int)desktopPosition.y );
}

void InputSystem::SetMouseClientPosition( Vector2 clientPosition )
{
	// Convert client to screen position
	// & Call the SetMouseScreenPosition( ... )

	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
	
	POINT mousePos;
	mousePos.x = (LONG)clientPosition.x;
	mousePos.y = (LONG)clientPosition.y;
	::ClientToScreen( hwnd, &mousePos );

	Vector2 mouseDesktopPos = Vector2( (float)mousePos.x, (float)mousePos.y );
	SetMouseScreenPosition( mouseDesktopPos );
}

void InputSystem::ShowCursor( bool show )
{
	::ShowCursor( show );
}

void InputSystem::MouseLockToScreen( bool lock )
{
	if( !lock )
	{
		::ClipCursor( nullptr );
	}
	else
	{
		HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
		
		RECT clientRect;
		::GetClientRect( hwnd, &clientRect );		// This should give you ( 0, 0, widthOfClient, heightOfClient )

		POINT offset;
		offset.x = 0;
		offset.y = 0;
		::ClientToScreen( hwnd, &offset );			// Top-Left corner in screen space

		clientRect.left		+= offset.x;
		clientRect.right	+= offset.x;
		clientRect.top		+= offset.y;
		clientRect.bottom	+= offset.y;

		::ClipCursor( &clientRect );
	}
}
