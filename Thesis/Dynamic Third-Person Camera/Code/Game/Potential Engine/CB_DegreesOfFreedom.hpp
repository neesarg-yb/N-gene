#pragma once
#include "Game/Potential Engine/CameraBehaviour.hpp"

class CB_DegreesOfFreedom : public CameraBehaviour
{
public:
	 CB_DegreesOfFreedom( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name = "DegreesOfFreedom" );
	~CB_DegreesOfFreedom();

public:
	float 		m_rotationSpeed		 = 20.f;							// Degrees per Second
	FloatRange	m_pitchRange		 = FloatRange( -60.f, 35.f );		// In Degrees

private:
	float const	m_distanceChangeSpeed = 3.5f;
	Vector3		m_spehicalCoordinates = Vector3( 10.f, 0.f, 0.f );		// ( radius, rotation, altitude )

public:
	void				PreUpdate () { }
	void				PostUpdate() { }
	CameraTargetPoint	Update( float deltaSeconds );

private:
	Vector3 GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude );
};