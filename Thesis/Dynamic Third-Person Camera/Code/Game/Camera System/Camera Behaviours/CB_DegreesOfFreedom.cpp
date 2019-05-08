#pragma once
#include "CB_DegreesOfFreedom.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

CB_DegreesOfFreedom::CB_DegreesOfFreedom( char const *name, CameraManager *manager, GameObject const *anchorOverwrite /* = nullptr */ )
	: CameraBehaviour( name, manager )
	, m_anchorOverwrite( anchorOverwrite )
{

}

CB_DegreesOfFreedom::~CB_DegreesOfFreedom()
{

}

void CB_DegreesOfFreedom::SetWorldPosition( float distanceFromAnchor, float rotationInDegrees, float altitudeInDegrees )
{
	Vector3 anchorWorldPosition		= GetAnchor()->m_transform.GetWorldPosition();
	Vector3 relativeCameraPosition	= GetPositionFromSpericalCoordinate( distanceFromAnchor, rotationInDegrees, altitudeInDegrees );
	Vector3 worldCameraPosition		= anchorWorldPosition + relativeCameraPosition;

	m_goalState.m_position			= worldCameraPosition;
}

void CB_DegreesOfFreedom::SetOrientationToLookAtAnchor()
{
	Vector3		anchorWorldPosition	= GetAnchor()->m_transform.GetWorldPosition();
	Matrix44	lookAtAnchorMatrix	= Matrix44::MakeLookAtView( anchorWorldPosition, m_goalState.m_position );
	Quaternion	cameraOrientation	= Quaternion::FromMatrix( lookAtAnchorMatrix ).GetInverse();

	m_goalState.m_orientation		= cameraOrientation;
}

void CB_DegreesOfFreedom::SetOffsetToWorldPosition( float localHorizontalOffset, float localVerticalOffset )
{
	Vector3 rightOfCameraInWorld = m_goalState.m_orientation.RotatePoint( Vector3::RIGHT );
	Vector3 upOfCameraInWorld	 = m_goalState.m_orientation.RotatePoint( Vector3::UP );
	Vector3 worldPositionOffset	 = ( rightOfCameraInWorld * localHorizontalOffset ) + ( upOfCameraInWorld * localVerticalOffset );

	m_goalState.m_position		+= worldPositionOffset;
}

void CB_DegreesOfFreedom::SetFOV( float cameraFOV )
{
	m_goalState.m_fov = cameraFOV;
}

GameObject const* CB_DegreesOfFreedom::GetAnchor() const
{
	if( m_anchorOverwrite != nullptr )
		return m_anchorOverwrite;
	else
		return m_anchor;
}

Vector3 CB_DegreesOfFreedom::GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude ) const
{
	return PolarToCartesian( radius, rotation, altitude);
}
