#pragma once
#include "CB_ZoomCamera.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

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

float CB_ZoomCamera::GetYawAngleForReferenceDirection( Vector2 const dirYp ) const
{
	// In left-handed coordinate system
	// When,
	//		yaw   = 0.0
	//		pitch = 0.0
	//	Reference Transform will be pointing in the direction of world Z-Axis
	return atan2fDegree( dirYp.x, dirYp.y );
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
	// Calculate for yaw
	{
		// Get the plane for Yaw calculations
		Vector3	const refPosWs = m_referenceTranform.GetWorldPosition();
		Plane3	const yawPlane = Plane3( Vector3::UP, refPosWs );

		// 2D Calculation
		// Get target & reference positions in Yaw Plane (Yp)
		Vector3 const targetProjected	= yawPlane.VectorProjection( targetWs );
		Vector3 const refPosProjected	= yawPlane.VectorProjection( refPosWs );
		Vector2 const targetYp			= Vector2( targetProjected.x, targetProjected.z );
		Vector2 const refPosYp			= Vector2( refPosProjected.x, refPosProjected.z );

		// Make the reference to look at the target
		Vector2 const refToTargetDispYp = targetYp - refPosYp;
		Vector2 const refToTargetDirYp  = ( refToTargetDispYp ).GetNormalized();
		m_refRotYaw = GetYawAngleForReferenceDirection( refToTargetDirYp );

		// Calculate extra rotation, so the camera looks at the target
		Vector2 const rightDirYpRs				= Vector2( 1.f, 0.f );
		Vector3 const camOffsetProjected		= yawPlane.VectorProjection( m_cameraOffset );
		Vector2 const camOffsetYp				= Vector2( camOffsetProjected.x, camOffsetProjected.z );
		float	const parallelDistRefToCamera	= Vector2::DotProduct( rightDirYpRs, camOffsetYp );
		float	const refToTargetDistanceYp		= refToTargetDispYp.GetLength();
		float	const additionalYawDegrees		= ( refToTargetDistanceYp != 0.f ) 
													? RadianToDegree( asinf( parallelDistRefToCamera / refToTargetDistanceYp ) )
													: 0.f;
		// Apply the extra rotation
		m_refRotYaw += ( additionalYawDegrees * -1.f );
	}

	// Calculate for pitch
	{

	}

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );
}

