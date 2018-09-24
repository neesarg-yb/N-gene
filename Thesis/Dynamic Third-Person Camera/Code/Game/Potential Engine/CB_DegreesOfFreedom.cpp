#pragma once
#include "CB_DegreesOfFreedom.hpp"

CB_DegreesOfFreedom::CB_DegreesOfFreedom( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name /* = "DegreesOfFreedom" */ )
	: CameraBehaviour( name )
	, m_distanceFromAnchor( distFromAnchor )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAnngle )
{

}

CB_DegreesOfFreedom::~CB_DegreesOfFreedom()
{

}

CameraTargetPoint CB_DegreesOfFreedom::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return CameraTargetPoint();
}

