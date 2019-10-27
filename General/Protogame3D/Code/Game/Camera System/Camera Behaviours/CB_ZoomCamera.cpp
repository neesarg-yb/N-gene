#pragma once
#include "CB_ZoomCamera.hpp"

CB_ZoomCamera::CB_ZoomCamera( Vector3 const &refPos, float fov, std::string const behaviorName, CameraManager const *manager )
	: CameraBehaviour( behaviorName, manager )
	, m_referenceTranform( refPos, Quaternion::IDENTITY, Vector3::ONE_ALL )
	, m_fov( fov )
{

}

CB_ZoomCamera::~CB_ZoomCamera()
{

}

void CB_ZoomCamera::PreUpdate()
{

}

void CB_ZoomCamera::PostUpdate()
{

}

CameraState CB_ZoomCamera::Update( float deltaSeconds, CameraState const &currentState )
{
	UNUSED( deltaSeconds );
	CameraState modifiedCamState = currentState;

	UpdateReferenceRotation();

	Matrix44 const	refTransformMat44 = m_referenceTranform.GetWorldTransformMatrix();
	
	modifiedCamState.m_position		= refTransformMat44.Multiply( m_cameraOffset, 1.0f );
	modifiedCamState.m_orientation	= m_referenceTranform.GetQuaternion();
	modifiedCamState.m_fov			= m_fov;
	
	return modifiedCamState;
}

void CB_ZoomCamera::UpdateReferenceRotation()
{
	// Get controller input
	Vector2 const mousePosDelta = m_inputSystem->GetMouseDelta();
	
	// Yaw
	float const deltaYawDegrees = m_mouseSensitivity * mousePosDelta.x;
	m_refRotYaw += deltaYawDegrees;

	// Pitch
	float const deltaPitchDegrees = m_mouseSensitivity * mousePosDelta.y;
	FloatRange allowedDeltaPitch = FloatRange( m_minPitchDegrees - m_refRotPitch, m_maxPitchDegrees - m_refRotPitch );
	if( allowedDeltaPitch.Includes( deltaPitchDegrees ) )
		m_refRotPitch += deltaPitchDegrees;
	
	Quaternion const yawRot		= Quaternion( Vector3::UP, m_refRotYaw );
	Quaternion const pitchRot	= Quaternion( yawRot.GetRight(), m_refRotPitch );

	// Set final rotation
	m_referenceTranform.SetQuaternion( yawRot.Multiply(pitchRot) );
}

void CB_ZoomCamera::SetReferencePosition( Vector3 const &refPosWs )
{
	m_referenceTranform.SetPosition( refPosWs );
}

void CB_ZoomCamera::SetCameraOffsetFromReference( Vector3 const &camOffset )
{
	m_cameraOffset = camOffset;
}

void CB_ZoomCamera::LookAtTargetPosition( Vector3 const &targetWs )
{
	UNUSED( targetWs );
}

