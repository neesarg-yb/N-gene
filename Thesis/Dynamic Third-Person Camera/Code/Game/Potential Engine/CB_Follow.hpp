#pragma once
#include "Game/Potential Engine/CB_DegreesOfFreedom.hpp"

class CB_Follow : public CB_DegreesOfFreedom
{
public:
	 CB_Follow(  float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name = "Follow" );
	~CB_Follow();

private:
	// Limits
	FloatRange	m_pitchRange				= FloatRange( -60.f, 35.f );	// In Degrees

	// Speed - To change variables by
	float		m_rotationSpeed				= 20.f;							// Degrees per Second
	float const	m_distanceChangeSpeed		= 3.5f;
	float const m_fovChangeSpeed			= 10.f;
	float const m_offsetChangeSpeed			= 1.f;

	// Spherical Coordinates
	float		m_distanceFromAnchor		= 0.f;
	float		m_rotationAroundAnchor		= 0.f;
	float		m_altitudeAroundAnchor		= 0.f;

	// Local Offsets
	float		m_localHorizontalOffset		= 0.f;
	float		m_localVerticalOffset		= 0.f;

	// Field of View
	float		m_fov						= m_currentState.m_fov;

public:
	void		PreUpdate()  { }
	void		PostUpdate() { }
	CameraState Update( float deltaSeconds );
};
