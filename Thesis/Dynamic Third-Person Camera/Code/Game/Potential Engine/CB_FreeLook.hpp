#pragma once
#include "Game/Potential Engine/CameraBehaviour.hpp"

class CB_FreeLook : public CameraBehaviour
{
public:
	 CB_FreeLook( float movementSpeed, float rotationSpeed, float minPitchAngle, float maxPitchAngle, char const *behaviourName, CameraManager const *manager );
	~CB_FreeLook();

public:
	float		m_movementSpeed	= 10.f;
	float		m_rotationSpeed	= 45.f;
	FloatRange	m_pitchRange	= FloatRange( 30.f, 100.f );
	
public:
	void		PreUpdate();
	void		PostUpdate();
	CameraState	Update( float deltaSeconds, CameraState const &currentState );
};
