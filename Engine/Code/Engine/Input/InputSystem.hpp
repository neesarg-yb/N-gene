#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"

enum eMouseButtons
{
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT,
	NUM_MOUSE_BUTTONS
};

enum eMouseModes
{
	MOUSE_MODE_ABSOLUTE = 0,
	MOUSE_MODE_RELATIVE,
	NUM_MOUSE_MODES
};

enum VK_Codes
{
	I			= 0x49,
	O			= 0x4F,
	P			= 0x50,
	T			= 0x54,
	N			= 0x4E,
	E			= 0x45,
	S			= 0x53,
	F			= 0x46,
	F1			= 0x70,				/* VK_F1 */
	UP			= 0x26,				/* VK_UP */
	DOWN		= 0x28,				/* VK_DOWN */
	LEFT		= 0x25,				/* VK_LEFT */
	RIGHT		= 0x27,				/* VK_RIGHT */
	SPACE		= 0x20,				/* VK_SPACE */
	BACK		= 0x08,				/* VK_BACK */
	DELETE_KEY	= 0x2E,				/* VK_DELETE */
	ESCAPE		= 0x1B,				/* VK_ESCAPE */
	ENTER		= 0x0D,				/* VK_RETURN */
	SHIFT		= 0x10				/* VK_SHIFT */
};

typedef KeyButtonState MouseButtonState;

class InputSystem
{
public:
	 InputSystem();
	~InputSystem();

	void	BeginFrame();
	void	EndFrame();

	// Clipboard
	static void GetStringFromClipboard( std::string &outStr );

	// Keyboard
	void	OnKeyPressed	( unsigned char keyCode );
	void	OnKeyReleased	( unsigned char keyCode );
	bool	IsKeyPressed	( unsigned char keyCode ) const;
	bool	WasKeyJustPressed	( unsigned char keyCode ) const;
	bool	WasKeyJustReleased	( unsigned char keyCode ) const;

	// Mouse
	void	OnMouseButtonPressed	( eMouseButtons buttonCode );
	void	OnMouseButtonReleased	( eMouseButtons buttonCode );
	bool	IsMouseButtonPressed	( eMouseButtons buttonToCheck ) const;
	bool	WasMousButtonJustPressed	( eMouseButtons buttonToCheck ) const;
	bool	WasMouseButtonJustReleased	( eMouseButtons buttonToCheck ) const;

	void	ShowCursor( bool show );
	void	MouseLockToScreen( bool lock );

	Vector2	GetMouseDelta();
	Vector2 GetMouseClientPosition();
	Vector2 GetCenterOfClientWindow();		// In Client Space

	void	SetMouseModeTo( eMouseModes mouseMode );
	void	SetMouseScreenPosition( Vector2 desktopPosition );
	void	SetMouseClientPosition( Vector2 clientPosition );

protected:
	void	UpdateMouse();
	void	UpdateKeyboard();
	void	UpdateController();

public:
	static const int	NUM_KEYBOARD_KEYS   = 256;
	XboxController		m_controller[ 4 ];

protected:
	KeyButtonState		m_keyStates[ NUM_KEYBOARD_KEYS ];
	MouseButtonState	m_mouseButtonStates[ NUM_MOUSE_BUTTONS ];

private:
	eMouseModes			m_mouseMode				 = MOUSE_MODE_ABSOLUTE;
	Vector2				m_mousePositionLastFrame = Vector2::ZERO;
	Vector2				m_mousePositionThisFrame = Vector2::ZERO;
};