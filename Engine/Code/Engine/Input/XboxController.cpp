#pragma once
#include "XboxController.hpp"

#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility


XBoxControllerStickState::XBoxControllerStickState() {
	correctedNormalizedPosition = Vector2(0.f, 0.f);
	magnitude = 0.f;
	orientationDegree = 0.f;
}


XboxController::XboxController()
{

}

XboxController::~XboxController()
{

}

void XboxController::UpdateControllerState() {

	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerNumber, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		// Clear last frame's keyJustPressed & keyJustReleased bool values for every button
		ClearJustPressedBools();

		/*mask = XINPUT_GAMEPAD_A*/
		UpdateButtonState(XBOX_BUTTON_A,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		UpdateButtonState(XBOX_BUTTON_B,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		UpdateButtonState(XBOX_BUTTON_X,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		UpdateButtonState(XBOX_BUTTON_Y,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);
		UpdateButtonState(XBOX_BUTTON_LEFT,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButtonState(XBOX_BUTTON_RIGHT,	xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButtonState(XBOX_BUTTON_UP,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButtonState(XBOX_BUTTON_DOWN,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateButtonState(XBOX_BUTTON_LB,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButtonState(XBOX_BUTTON_RB,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButtonState(XBOX_BUTTON_LS,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButtonState(XBOX_BUTTON_RS,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButtonState(XBOX_BUTTON_START,	xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);
		UpdateButtonState(XBOX_BUTTON_BACK,		xboxControllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		
		UpdateStickState(XBOX_STICK_LEFT,		xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY );
		UpdateStickState(XBOX_STICK_RIGHT,		xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY );

		UpdateTriggerState(XBOX_TRIGGER_LEFT,	xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTriggerState(XBOX_TRIGGER_RIGHT,	xboxControllerState.Gamepad.bRightTrigger);
	}
}

void XboxController::UpdateButtonState(XboxControllerButtonID buttonID, unsigned short rawButtonValues, unsigned short mask) {
	
	// key is down
	if( (rawButtonValues & mask) == mask ) {

		// Button is pressed, change state
		if( m_xboxButtonStates[buttonID].keyIsDown != true) {
			m_xboxButtonStates[buttonID].keyJustPressed = true;
		}
		m_xboxButtonStates[buttonID].keyIsDown = true;
	}
	else { // key is up

		// Button is not pressed, change state
		if( m_xboxButtonStates[buttonID].keyIsDown == true ) {
			m_xboxButtonStates[buttonID].keyJustReleasesd = true;
		}
		m_xboxButtonStates[buttonID].keyIsDown = false;
	}
}

void XboxController::UpdateTriggerState(XboxControllerTriggerID triggerID , unsigned char rawTriggerValue) {
	// Convert rawTriggerValue from [0, 255] to [0, 1.0] range
	m_xboxTriggerStates[ triggerID ] = RangeMapFloat( (float)rawTriggerValue, 0.f, 255.f, 0.f, 1.f );
}

void XboxController::UpdateStickState(XboxControllerStickID stickID , short rawXValue, short rawYValue) {
	// Normalize
	// Map rawX, rawY from [-32768, +32767]
	float rawNormalizedX = RangeMapFloat(rawXValue, -32768.f, 32767.f, -1.f, 1.f);
	float rawNormalizedY = RangeMapFloat(rawYValue, -32768.f, 32767.f, -1.f, 1.f);

	// Get rawPolar coordinates (R, theta)
	float R = sqrtf( (rawNormalizedX*rawNormalizedX) + (rawNormalizedY*rawNormalizedY) );
	float theta = atan2fDegree( rawNormalizedY, rawNormalizedX );

	// Correct R from dead-zone. [16%, 95%] -> [0%, 100%]
	float correctedR = RangeMapFloat(R, 0.16f, 0.95f, 0.f, 1.f);
	correctedR = ClampFloat01(correctedR);

	// Compute corrected (x, y) from (correctedR, theta)
	float x_final = correctedR * CosDegree(theta);
	float y_final = correctedR * SinDegree(theta);

	// Set values to member state variable
	m_xboxStickStates[ stickID ].correctedNormalizedPosition = Vector2( x_final, y_final );
	m_xboxStickStates[ stickID ].magnitude = correctedR;
	m_xboxStickStates[ stickID ].orientationDegree = theta;
}

void XboxController::ClearJustPressedBools() {
	for(int i=0; i<NUM_XBOX_BUTTONS; i++) {
		m_xboxButtonStates[i].keyJustPressed = false;
		m_xboxButtonStates[i].keyJustReleasesd = false;
	}
}