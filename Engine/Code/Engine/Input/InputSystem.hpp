#pragma once
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Math/Vector2.hpp"
#include "Engine/KeyButtonState.hpp"
#include "Engine/XboxController.hpp"

enum VK_Codes
{
	ESCAPE = 0x1B,				/* VK_ESCAPE */
	I = 0x49,
	O = 0x4F,
	P = 0x50,
	T = 0x54,
	N = 0x4E,
	E = 0x45,
	S = 0x53,
	F = 0x46,
	F1 = 0x70,					/* VK_F1 */
	UP = 0x26,					/* VK_UP */
	LEFT = 0x25,				/* VK_LEFT */
	RIGHT = 0x27,				/* VK_RIGHT */
	SPACE = 0x20				/* VK_SPACE */
};


class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void BeginFrame();
	void EndFrame();
	
	void OnKeyPressed( unsigned char keyCode );
	void OnKeyReleased( unsigned char keyCode );
	bool IsKeyPressed( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

public:
	static const int			NUM_KEYS		= 256;
	XboxController  m_controller[ 4 ];

protected:
	void UpdateKeyboard();
	void UpdateController();

protected:
	KeyButtonState	m_keyStates[ NUM_KEYS ];
};