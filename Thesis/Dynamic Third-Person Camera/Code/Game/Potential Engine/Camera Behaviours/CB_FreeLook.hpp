#pragma once
#include "Game/Potential Engine/Camera System/CameraBehaviour.hpp"

enum eFreeLookInputSource
{
	USE_CONTROLLER_FL,
	USE_KEYBOARD_MOUSE_FL
};

class CB_FreeLook : public CameraBehaviour
{
public:
	 CB_FreeLook( float movementSpeed, float rotationSpeed, float minPitchAngle, float maxPitchAngle, char const *behaviourName, CameraManager const *manager, eFreeLookInputSource inputSource );
	~CB_FreeLook();

public:
	float		m_movementSpeed	= 10.f;
	float		m_rotationSpeed	= 45.f;
	FloatRange	m_pitchRange	= FloatRange( 30.f, 100.f );

private:
	eFreeLookInputSource m_inputSource = USE_CONTROLLER_FL;
	
public:
	// Behaviour Update(s)
	void		PreUpdate();
	void		PostUpdate();
	CameraState	Update( float deltaSeconds, CameraState const &currentState );

	// Input Source
	inline void SetInputSource( eFreeLookInputSource const newSource )	{ m_inputSource = newSource; }
	inline eFreeLookInputSource GetCurrentInputSource() const			{ return m_inputSource; }

private:
	void ProcessInput( Vector2 &outXZMovement, Vector2 &outXYRotation, float &outYAxisChange );		// Notes the input, this frame..
};
