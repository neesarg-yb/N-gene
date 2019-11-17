#pragma once
#include "CB_ZoomCamera.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
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

	Matrix44 const   refTransformMat44	= m_referenceTranform.GetWorldTransformMatrix();
	Quaternion const referenceRot		= m_referenceTranform.GetQuaternion();
	Quaternion const extraYawRotation	= Quaternion( referenceRot.GetUp(), m_camYawExtraRot );

	modifiedCamState.m_position		= refTransformMat44.Multiply( m_cameraOffset, 1.0f );
	modifiedCamState.m_orientation	= referenceRot.Multiply( extraYawRotation );
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

void CB_ZoomCamera::GetExtraRotationForReticleOffset( Vector2 const &reticlePos, Vector2 const screenDimensions, float &yawDegrees_out, float &pitchDegrees_out )
{
	if( screenDimensions.x == 0.f || screenDimensions.y == 0.f )
		return;

	CameraState	const &currCameraState	= m_manager->GetCurrentCameraState();
	float		const  fovRadians		= DegreeToRadian( currCameraState.m_fov );
	float		const  aspectRatio		= screenDimensions.x / screenDimensions.y;

	float yawRadians = 0.f;
	{
		float const halfTanFov = tanf( fovRadians * 0.5f );
		yawRadians = atanf( halfTanFov * ( reticlePos.x / screenDimensions.x ) * 2.f );
	}

	float pitchRadians = 0.f;
	{
		float const halfTanVertAngle = tanf( fovRadians * 0.5f / aspectRatio );
		pitchRadians = atanf( halfTanVertAngle * ( reticlePos.y / screenDimensions.y ) * 2.f );
	}

	yawDegrees_out	 = RadianToDegree( yawRadians );
	pitchDegrees_out = RadianToDegree( pitchRadians );
}

void CB_ZoomCamera::SetReferencePosition( Vector3 const &refPosWs )
{
	m_referenceTranform.SetPosition( refPosWs );
}

void CB_ZoomCamera::SetCameraOffsetFromReference( Vector3 const &camOffset )
{
	m_cameraOffset = camOffset;
}

void CB_ZoomCamera::SetCameraYawExtraRotation( float yawDegreesExtra )
{
	m_camYawExtraRot = yawDegreesExtra;
}

void CB_ZoomCamera::LookAtTargetPosition( Vector3 const &targetWs, Vector2 const &reticlePos, Vector2 const &screenDimensions )
{
	Vector3 const refPosWs			= m_referenceTranform.GetWorldPosition();
	Vector3 const refToTargetDisp	= targetWs - refPosWs;

	float yawDegreesReticleOffset = 0.f;
	float pitchDegreesReticleOffset = 0.f;
	{
		GetExtraRotationForReticleOffset( reticlePos, screenDimensions, yawDegreesReticleOffset, pitchDegreesReticleOffset );
		ConsolePrintf( "x-offset = %.1f, y-offset = %.1f degrees", yawDegreesReticleOffset, pitchDegreesReticleOffset );
	}
	
	// Calculate for yaw
	{
		Vector3 const refRightDir = Vector3::RIGHT;
		Vector3 const refFrontDir = Vector3::FRONT;

		// 2D Calculation in Yaw Plane (Yp)
		float	const x = Vector3::DotProduct( refRightDir, refToTargetDisp );
		float	const y = Vector3::DotProduct( refFrontDir, refToTargetDisp );
		Vector2	const refToTargetDispYp = Vector2( x, y );

		// Make the reference to look at the target
		//
		//      (ref front dir)
		//           Y |              
		//             |     * target 
		//             |    /         
		//             |   /          
		//             |  /            a = m_refRotYaw = atan( x / y );		// Referece rotates clock-wise, if viewed from Yp
		//             |a/            
		//  Yaw Plane  |/_____________ (ref right dir)
		//    (Yp)     * ref         X 
		m_refRotYaw = atan2fDegree( refToTargetDispYp.x, refToTargetDispYp.y );

		// Calculate extra rotation, so the camera looks at the target
		float	const cameraOffsetYp	= Vector3::DotProduct( refRightDir, m_cameraOffset );
		float	const refToTargetDistYp	= refToTargetDispYp.GetLength();
		float	const additionalYawDegrees = ( refToTargetDistYp != 0.f ) 
												? RadianToDegree( asinf( cameraOffsetYp / refToTargetDistYp ) )
												: 0.f;
		// Apply the extra rotation
		m_refRotYaw += ( -1.f * additionalYawDegrees );
	}

	// Calculate for pitch
	{
		Quaternion	const refPostYawRot	= Quaternion( Vector3::UP, m_refRotYaw );
		Vector3		const refFrontDir	= refPostYawRot.GetFront();
		Vector3		const refUpDir		= refPostYawRot.GetUp();
		
		// For 2D calculations in Pitch Plane (Pp)
		float	const x = Vector3::DotProduct( refFrontDir, refToTargetDisp );
		float	const y = Vector3::DotProduct( refUpDir, refToTargetDisp );
		Vector2	const refToTargetDispPp	= Vector2( x, y );

		// Make the reference to look at target (as far as pitch is concerned)
		//
		//  Pitch Plane             (ref up dir)
		//      (Pp)                     | Y
		//                  target *     | 
		//                          \    | 
		//                           \   | 
		//                            \  | 
		//           m_refRotPitch = a \ |  a = -atan( y / x );		// Reference rotates anti clock-wise, if viewed from Pp
		//                ______________\| 
		// (ref front dir) X             * ref
		m_refRotPitch = -1.0f * atan2fDegree( refToTargetDispPp.y, refToTargetDispPp.x );

		// Calculate extra rotation, so that the camera looks at the target
		float	const refToTargetDistPp = refToTargetDispPp.GetLength();
		float	const cameraOffsetPp	= Vector3::DotProduct( refUpDir, m_cameraOffset );
		float	const additionaPitchDegrees = ( refToTargetDistPp != 0.f )
												? RadianToDegree( asinf( cameraOffsetPp / refToTargetDistPp ) )
												: 0.f;
		// Apply extra rotation
		m_refRotPitch += additionaPitchDegrees;
	}

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );

	TODO( "Restrict pitch as per m_minPitchDegrees & m_maxPitchDegrees" );
}
