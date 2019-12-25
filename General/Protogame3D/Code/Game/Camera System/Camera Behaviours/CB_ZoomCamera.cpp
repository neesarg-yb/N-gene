#pragma once
#include "CB_ZoomCamera.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Core/Window.hpp"
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

	Matrix44 const refTransformMat44 = m_referenceTranform.GetWorldTransformMatrix();
	modifiedCamState.m_fov			 = m_fov;
	modifiedCamState.m_position		 = refTransformMat44.Multiply( m_cameraOffset, 1.0f );
	modifiedCamState.m_orientation	 = m_referenceTranform.GetQuaternion();

	{
		// Extra yaw rotation
		Quaternion const extraYawRotation	= Quaternion( modifiedCamState.m_orientation.GetUp(), m_camYawExtraRot );
		modifiedCamState.m_orientation		= modifiedCamState.m_orientation.Multiply( extraYawRotation );

		// Extra pitch rotation
		Quaternion const extraPitchRotation = Quaternion( modifiedCamState.m_orientation.GetRight(), m_camPitchExtraRot );
		modifiedCamState.m_orientation		= modifiedCamState.m_orientation.Multiply( extraPitchRotation );
	}
	
	m_cameraState = modifiedCamState;
	
	return m_cameraState;
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

void CB_ZoomCamera::SetCameraPitchExtraRotation( float pitchDegreesExtra )
{
	m_camPitchExtraRot = pitchDegreesExtra;
}

void CB_ZoomCamera::SetReticleOffset( IntVector2 reticleOffsetSs )
{
	// Set reticle offset
	m_reticleOffset = reticleOffsetSs;
	
	// Set reticle degrees
	{
		IntVector2 const screenCenter = Window::GetInstance()->GetDimensions() * 0.5f;

		Vector3 const screenCenterWs = m_cameraState.GetWorldCoordFromScreen( screenCenter, m_camera->GetCameraNear(), m_camera->GetCameraFar() );
		Vector3 const yawReticleWs	 = m_cameraState.GetWorldCoordFromScreen( screenCenter + IntVector2(m_reticleOffset.x, 0), m_camera->GetCameraNear(), m_camera->GetCameraFar() );
		Vector3 const pitchReticleWs = m_cameraState.GetWorldCoordFromScreen( screenCenter + IntVector2(0, m_reticleOffset.y), m_camera->GetCameraNear(), m_camera->GetCameraFar() );
		Vector3 const cameraPosWs	 = m_cameraState.GetTransform().GetWorldPosition();

		Vector3 const screenCenterDir = ( screenCenterWs - cameraPosWs ).GetNormalized();
		Vector3 const yawReticleDir	  = ( yawReticleWs   - cameraPosWs ).GetNormalized();
		Vector3 const pitchReticleDir = ( pitchReticleWs - cameraPosWs ).GetNormalized();

		float const yawDotProduct = ClampFloat( Vector3::DotProduct(screenCenterDir, yawReticleDir), -1.f, 1.f );
		m_reticleYawDegrees = RadianToDegree( acosf( yawDotProduct ) );
		m_reticleYawDegrees = copysignf( m_reticleYawDegrees, (float)m_reticleOffset.x );
		
		float const pitchDotProduct = ClampFloat( Vector3::DotProduct( screenCenterDir, pitchReticleDir), -1.f, 1.f );
		m_reticlePitchDegrees = RadianToDegree( acosf( pitchDotProduct ) );
		m_reticlePitchDegrees = -1.f * copysignf( m_reticlePitchDegrees, (float)m_reticleOffset.y );	// -1 because the *negative* y-screen-offset means, the pitch rotation according to the left hand direction
	}
}

void CB_ZoomCamera::LookAtTargetPosition( Vector3 const &targetWs )
{
	Vector3 const refPosWs			= m_referenceTranform.GetWorldPosition();
	Vector3 const refToTargetDisp	= targetWs - refPosWs;
	
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
		m_refRotYaw += -1.f * additionalYawDegrees;
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

	SetCameraYawExtraRotation( -m_reticleYawDegrees );
	SetCameraPitchExtraRotation( -m_reticlePitchDegrees );

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );

	TODO( "Restrict pitch as per m_minPitchDegrees & m_maxPitchDegrees" );
}
