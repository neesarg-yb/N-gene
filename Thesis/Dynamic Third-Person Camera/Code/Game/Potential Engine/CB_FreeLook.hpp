#pragma once
#include "Game/Potential Engine/CameraBehaviour.hpp"

class CB_FreeLook : public CameraBehaviour
{
public:
	 CB_FreeLook( char const *behaviourName );
	~CB_FreeLook();

public:
	float		m_movementSpeed	= 10.f;
	float		m_rotationSpeed	= 45.f;
	FloatRange	m_pitchRange	= FloatRange( 30.f, 100.f );
	
public:
	void				PreUpdate();
	void				PostUpdate();
	CameraTargetPoint	Update( float deltaSeconds );
};