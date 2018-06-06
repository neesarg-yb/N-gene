#pragma once

#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility

#include "Engine/Math/Vector2.hpp"
#include "Engine/KeyButtonState.hpp"
#include "Engine/Math/MathUtil.hpp"


struct XBoxControllerStickState {
	XBoxControllerStickState();

	Vector2 correctedNormalizedPosition;	// (x, y) = ( [-1,1] , [-1,1] )
	float magnitude;						// range: [0, 1]
	float orientationDegree;				// theta in degrees
};



enum XboxControllerButtonID
{
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,
	XBOX_BUTTON_LEFT,
	XBOX_BUTTON_RIGHT,
	XBOX_BUTTON_UP,
	XBOX_BUTTON_DOWN,
	XBOX_BUTTON_RB,
	XBOX_BUTTON_LB,
	XBOX_BUTTON_RS,
	XBOX_BUTTON_LS,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	NUM_XBOX_BUTTONS
};

enum XboxControllerTriggerID
{

	XBOX_TRIGGER_LEFT,
	XBOX_TRIGGER_RIGHT,
	NUM_XBOX_TRIGGERS
};

enum XboxControllerStickID
{
	XBOX_STICK_LEFT,
	XBOX_STICK_RIGHT,
	NUM_XBOX_STICKS
};

class XboxController
{
public:
	XboxController();
	~XboxController();

	int m_controllerNumber;

	KeyButtonState  m_xboxButtonStates[ NUM_XBOX_BUTTONS ];
	XBoxControllerStickState m_xboxStickStates[ NUM_XBOX_STICKS ];
	float m_xboxTriggerStates[ NUM_XBOX_TRIGGERS ];

	void UpdateControllerState();
	void UpdateButtonState(XboxControllerButtonID buttonID, unsigned short rawButtonValues, unsigned short mask);
	void UpdateStickState(XboxControllerStickID stickID , short rawXValue, short rawYValue);
	void UpdateTriggerState(XboxControllerTriggerID triggerID , unsigned char rawTriggerValue);

private:
	// Clear last frame's keyJustPressed & keyJustReleased bool values for every button
	void ClearJustPressedBools();
};