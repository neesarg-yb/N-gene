#pragma once
#include "Game/Potential Engine/Camera Behaviours/CB_DegreesOfFreedom.hpp"

class CB_ProportionalController : public CB_DegreesOfFreedom
{
public:
	 CB_ProportionalController( char const *name, CameraManager const *manager );
	~CB_ProportionalController();

private:
	// Limits
	FloatRange	m_pitchRange				= FloatRange( 30.f, 100.f );	// In Degrees

	// Speed - To change variables by
	float		m_rotationSpeed				= 26.f;							// Degrees per Second
	float const	m_distanceChangeSpeed		= 3.5f;
	float const m_fovChangeSpeed			= 10.f;
	float const m_offsetChangeSpeed			= 1.f;

	// Spherical Coordinates
	float		m_distanceFromAnchor		= 5.f;
	float		m_rotationAroundAnchor		= 0.f;
	float		m_altitudeAroundAnchor		= 70.f;

	// Local Offsets
	float		m_localHorizontalOffset		= 0.f;
	float		m_localVerticalOffset		= 0.f;

	// Field of View
	float		m_fov						= m_goalState.m_fov;

	// Proportional Controller
	bool		m_mpcEnabled				= false;
	float		m_controllingFactor			= 8.5f;
	float		m_accelerationLimit			= 35.f;							// Units per second^2
	float		m_leadFactor				= 1.f;

public:
	void		PreUpdate();
	void		PostUpdate();
	CameraState	Update( float deltaSeconds, CameraState const &currentState );
};
