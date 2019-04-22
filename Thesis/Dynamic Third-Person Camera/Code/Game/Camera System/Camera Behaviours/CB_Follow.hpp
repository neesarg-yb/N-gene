#pragma once
#include "Game/Camera System/Camera Behaviours/CB_DegreesOfFreedom.hpp"
#include "Game/Camera System/Camera Constraints/CC_HandoverToFollow.hpp"
#include "Game/Camera System/Camera Constraints/CC_HandoverToShoulder.hpp"

struct CameraContext;
struct ConstraintSuggestionOverwriteState
{
public:
	bool	m_playerHasCameraControl = false;			// If the player currently has the control over camera
	Vector3	m_playerPositionOnBegin	 = Vector3::ZERO;	// Position at where the player overtook the camera control
	double	m_timeElapsedSecondsIdle = 0.0;				// Time elapsed since the player had camera controls but is not doing anything with it

public:
	ConstraintSuggestionOverwriteState() { }
	ConstraintSuggestionOverwriteState( bool playerHasControl, Vector3 playerPositionOnBegin );
};

class CB_Follow : public CB_DegreesOfFreedom
{
	friend CC_HandoverToFollow;
	friend CC_HandoverToShoulder;

public:
	 CB_Follow( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name, CameraManager *manager );
	~CB_Follow();

public:
	// Speed - To change variables by
	float const	m_rotationSpeed				= 50.f;							// Degrees per Second
	float const	m_distanceChangeSpeed		= 3.5f;
	float const m_fovChangeSpeed			= 10.f;
	float const m_offsetChangeSpeed			= 1.f;

private:
	// Limits
	FloatRange	m_pitchRange				= FloatRange( -60.f, 35.f );	// In Degrees

	// Spherical Coordinates													_______
	float		m_distanceFromAnchor		= 0.f;							// |current|
	float		m_rotationAroundAnchor		= 0.f;							// | polar |
	float		m_altitudeAroundAnchor		= 0.f;							// |_______|
	
	// Camera reorientation around anchor
	bool		m_reorientCameraRotation	= false;
	float		m_reorientTargetRotDegrees	= 0.f;
	uint		m_framesSinceReorientBegan	= 1U;							// How many frames has passed since the reorientation procedure has began, most recently
	float		m_minSpeedReqToReorient		= 10.f;							// Min speed of anchor, after which the reorientation starts
	float		m_reorientAfterDegreeDiff	= 145.f;						// Degree difference between camera forward and anchor forward
	float 		m_reorientDotThreshold		= 0.f;

	// Local Offsets
	float		m_localHorizontalOffset		= 0.f;
	float		m_localVerticalOffset		= 0.f;

	// Field of View
	float		m_fov						= m_goalState.m_fov;

	float		m_constrainTakeOverTime		= 1.0;							// Seconds
	ConstraintSuggestionOverwriteState m_suggestionOverwriteState;

public:
	void		PreUpdate()  { }
	void		PostUpdate() { }
	CameraState Update( float deltaSeconds, CameraState const &currentState );

	void		SetupForIncomingHandover( float rotationAroundAnchor, float altitudeFromTop );
	void		SuggestChangedPolarCoordinate( float radius, float rotation, float altitude );	// A Camera Constrain can used this; meant to be used by CC_ModifiedConeRaycast
	
	bool		StartCameraReorientation();
	void		StopCameraReorientation();

private:
	void	GetPlayerInput( float &distChange_out, float &rotChange_out, float &altChange_out, float &hOffsetChange_out, float &vOffsetChange_out, float &fovChange_out ) const;
	bool	CheckToTurnOnConstraintSuggestions( ConstraintSuggestionOverwriteState const &suggestionOverwriteState ) const;

	void	CartesianToPolarTest( CameraState const &camState ) const;
	float	GetRotationToFaceXZDirection( Vector2 const &xzDir ) const;
};
