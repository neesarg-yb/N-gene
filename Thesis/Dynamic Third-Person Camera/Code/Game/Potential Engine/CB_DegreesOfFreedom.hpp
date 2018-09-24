#pragma once
#include "Game/Potential Engine/CameraBehaviour.hpp"

class CB_DegreesOfFreedom : public CameraBehaviour
{
public:
	 CB_DegreesOfFreedom( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name = "DegreesOfFreedom" );
	~CB_DegreesOfFreedom();

public:
	float		m_distanceFromAnchor = 10.f;							// World Units
	float		m_rotationSpeed		 = 20.f;							// Degrees per Second
	FloatRange	m_pitchRange		 = FloatRange( -60.f, 35.f );		// In Degrees

public:
	void				PreUpdate () { }
	void				PostUpdate() { }
	CameraTargetPoint	Update( float deltaSeconds );
};