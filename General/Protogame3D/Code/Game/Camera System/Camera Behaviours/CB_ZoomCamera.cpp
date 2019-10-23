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
	CameraState modifiedCamState = currentState;

	UpdateReferenceRotation( deltaSeconds );

	Matrix44 const	refTransformMat44 = m_referenceTranform.GetWorldTransformMatrix();
	
	modifiedCamState.m_position		= refTransformMat44.Multiply( m_cameraOffset, 1.0f );
	modifiedCamState.m_orientation	= m_referenceTranform.GetQuaternion();
	modifiedCamState.m_fov			= m_fov;
	
	return modifiedCamState;
}

void CB_ZoomCamera::UpdateReferenceRotation( float deltaSeconds )
{
	// Get controller input
	Vector2 const rightStickXBox = m_inputSystem->m_controller[0].m_xboxStickStates[XBOX_STICK_RIGHT].correctedNormalizedPosition;

	// Set reference rotation
	Quaternion refRot = m_referenceTranform.GetQuaternion();

	// Delta Rotation
	Quaternion yawRot = Quaternion( Vector3::UP, m_rotSpeed * rightStickXBox.x * deltaSeconds );
	Quaternion pitchRot = Quaternion( m_referenceTranform.GetWorldTransformMatrix().GetIColumn(), m_rotSpeed * rightStickXBox.y * deltaSeconds );

	// Add rotation
	refRot = refRot.Multiply( yawRot );
	refRot = refRot.Multiply( pitchRot );

	// Set final
	m_referenceTranform.SetQuaternion( refRot );
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

