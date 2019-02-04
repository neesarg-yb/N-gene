#pragma once
#include "Game/Camera System/Camera Behaviours/CB_DegreesOfFreedom.hpp"

class CB_Follow : public CB_DegreesOfFreedom
{
public:
	 CB_Follow( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name, CameraManager const *manager );
	~CB_Follow();

private:
	// Limits
	FloatRange	m_pitchRange				= FloatRange( -60.f, 35.f );	// In Degrees

	// Speed - To change variables by
	float		m_rotationSpeed				= 50.f;							// Degrees per Second
	float const	m_distanceChangeSpeed		= 3.5f;
	float const m_fovChangeSpeed			= 10.f;
	float const m_offsetChangeSpeed			= 1.f;

	// Spherical Coordinates													_______
	float		m_distanceFromAnchor		= 0.f;							// |current|
	float		m_rotationAroundAnchor		= 0.f;							// | polar |
	float		m_altitudeAroundAnchor		= 0.f;							// |_______|
	bool		m_reorientCameraRotation	= false;
	float		m_reorientDotThreshold		= 0.f;

	// Local Offsets
	float		m_localHorizontalOffset		= 0.f;
	float		m_localVerticalOffset		= 0.f;

	// Field of View
	float		m_fov						= m_goalState.m_fov;

public:
	void		PreUpdate()  { }
	void		PostUpdate() { }
	CameraState Update( float deltaSeconds, CameraState const &currentState );

	void		SuggestChangedPolarCoordinate( float radius, float rotation, float altitude );	// A Camera Constrain can used this; meant to be used by CC_ModifiedConeRaycast
	
private:
	void	GetPlayerInput( float &distChange_out, float &rotChange_out, float &altChange_out, float &hOffsetChange_out, float &vOffsetChange_out, float &fovChange_out ) const;
	void	CartesianToPolarTest( CameraState const &camState ) const;
	float	GetRotationToFaceXZDirection( Vector2 const &xzDir ) const;
};
