#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"

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
	LEFT		= 0x25,				/* VK_LEFT */
	RIGHT		= 0x27,				/* VK_RIGHT */
	SPACE		= 0x20,				/* VK_SPACE */
	BACK		= 0x08,				/* VK_BACK */
	DELETE_KEY	= 0x2E,				/* VK_DELETE */
	ESCAPE		= 0x1B,				/* VK_ESCAPE */
	ENTER		= 0x0D				/* VK_RETURN */
};


class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void BeginFrame();
	void EndFrame();
	
	void OnKeyPressed	( unsigned char keyCode );
	void OnKeyReleased	( unsigned char keyCode );
	bool IsKeyPressed	( unsigned char keyCode ) const;
	bool WasKeyJustPressed	( unsigned char keyCode ) const;
	bool WasKeyJustReleased	( unsigned char keyCode ) const;

protected:
	void UpdateKeyboard();
	void UpdateController();

public:
	static const int			NUM_KEYS   = 256;
	XboxController				m_controller[ 4 ];

protected:
	KeyButtonState				m_keyStates[ NUM_KEYS ];
};