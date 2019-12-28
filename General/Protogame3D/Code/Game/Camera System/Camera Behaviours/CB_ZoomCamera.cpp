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
	m_cameraState = Update( 0.f, m_manager->GetCurrentCameraState() );
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
	Vector2 const mousePosDelta = m_inputSystem ? m_inputSystem->GetMouseDelta() : Vector2::ZERO;
	
	// Yaw
	// float const deltaYawDegrees = m_mouseSensitivity * mousePosDelta.x;
	// m_refRotYawWs += deltaYawDegrees;

	// Pitch
	float const deltaPitchDegrees = m_mouseSensitivity * mousePosDelta.y;
	FloatRange allowedDeltaPitch = FloatRange( m_minPitchDegrees - m_refRotPitchWs, m_maxPitchDegrees - m_refRotPitchWs );
	if( allowedDeltaPitch.Includes( deltaPitchDegrees ) )
		m_refRotPitchWs += deltaPitchDegrees;
	
	Quaternion const yawRot			= Quaternion( Vector3::UP, m_refRotYawWs );
	Quaternion const pitchRot		= Quaternion( yawRot.GetRight(), m_refRotPitchWs );
	// Quaternion const yawRticleRot	= Quaternion( Vector3::UP, m_refRotYawWs + m_reticleYawDegreesWs );
	// Quaternion const pitchRot		= Quaternion( yawRticleRot.GetRight(), m_refRotPitchWs );

	// Set final rotation
	// m_referenceTranform.SetQuaternion( yawRot.Multiply(pitchRot) );
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
	// Set reticle offset - pixels
	m_reticleOffset = reticleOffsetSs;
	
	// Set reticle offset - degrees
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
		m_reticleYawDegreesWs = RadianToDegree( acosf( yawDotProduct ) );
		m_reticleYawDegreesWs = copysignf( m_reticleYawDegreesWs, (float)m_reticleOffset.x );
		
		float const pitchDotProduct = ClampFloat( Vector3::DotProduct( screenCenterDir, pitchReticleDir), -1.f, 1.f );
		m_reticlePitchDegreesWs = RadianToDegree( acosf( pitchDotProduct ) );
		m_reticlePitchDegreesWs = -1.f * copysignf( m_reticlePitchDegreesWs, (float)m_reticleOffset.y );	// -1 because the *negative* y-screen-offset means, the pitch rotation according to the left hand direction
	}
}

void CB_ZoomCamera::LookAtTargetPosition( Vector3 const &targetWs )
{
	TODO("FIXME: Pitch is inaccurate. Yaw is accurate only when the camera's y-coord is similar to target's y-coord.");

	Vector3 const refPosWs			= m_referenceTranform.GetWorldPosition();
	Vector3 const refToTargetDisp	= targetWs - refPosWs;
	
	Quaternion refFinalRotationWs	= Quaternion::IDENTITY;
	Quaternion reticleRotationWs	= Quaternion::IDENTITY;
	
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
		float const refRotLookAtTargetYp = atan2fDegree( refToTargetDispYp.x, refToTargetDispYp.y );	// Without any reticle offset, makes the reference-yaw look at target
		m_refRotYawWs  = refRotLookAtTargetYp;
		m_refRotYawWs -= m_reticleYawDegreesWs;															// Because we have the reticle offset, we need to rotate by this much, extra

		// Calculate extra rotation, so the camera looks at the target
		//
		//          (up dir)                  _____________
		//            Y |                    |    Note:    |
		//              |   (front dir)      | Left handed |
		//              |    /               |    system   |
		//              |   / Z               -------------
		//              |  /                       
		//              | /                        
		//  World Space |/_____________ (right dir)
		//     (Ws)     * origin      X            
		Vector3	const yawRightDir			= Quaternion( Vector3::UP, m_reticleYawDegreesWs ).RotatePoint( Vector3::RIGHT );
		float	const cameraProjOnYawRight	= Vector3::DotProduct( yawRightDir, m_cameraOffset );
		float	const refToTargetDistYp		= refToTargetDispYp.GetLength();
		float	const additionalYawDegrees	= ( refToTargetDistYp != 0.f ) 
												? RadianToDegree( asinf( cameraProjOnYawRight / refToTargetDistYp ) )
												: 0.f;
		// Apply the extra rotation
		m_refRotYawWs += -1.f * additionalYawDegrees;

		refFinalRotationWs	= refFinalRotationWs.Multiply( Quaternion( Vector3::UP, m_refRotYawWs ) );
		reticleRotationWs	= reticleRotationWs.Multiply( Quaternion( Vector3::UP, m_refRotYawWs + m_reticleYawDegreesWs ) );
	}

	// Calculate for pitch
	{
		Quaternion	const refPostYawRot	= Quaternion( Vector3::UP, m_refRotYawWs + m_reticleYawDegreesWs );
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
		float const refRotLookAtTargetPp = atan2fDegree( refToTargetDispPp.y, refToTargetDispPp.x );	// Without any reticle offset, makes the reference-pitch look at target
		m_refRotPitchWs  = -1.f * refRotLookAtTargetPp;
		m_refRotPitchWs -= m_reticlePitchDegreesWs;														// Because we have the reticle offset, we need to rotate by this much, extra

		// Calculate extra rotation, so that the camera looks at the target
		//
		//          (up dir)                  _____________
		//            Y |                    |    Note:    |
		//              |   (front dir)      | Left handed |
		//              |    /               |    system   |
		//              |   / Z               -------------
		//              |  /                       
		//              | /                        
		//  World Space |/_____________ (right dir)
		//     (Ws)     * origin      X            
		Vector3	const pitchUpDir			= Quaternion( Vector3::RIGHT, -m_reticlePitchDegreesWs ).RotatePoint( Vector3::UP );
		float	const cameraProjOnPitchUp	= Vector3::DotProduct( pitchUpDir, m_cameraOffset );
		float	const refToTargetDistPp		= refToTargetDispPp.GetLength();
		float	const additionaPitchDegrees = ( refToTargetDistPp != 0.f )
												? RadianToDegree( asinf( cameraProjOnPitchUp / refToTargetDistPp ) )
												: 0.f;
		// Apply extra rotation
		m_refRotPitchWs += additionaPitchDegrees;

		refFinalRotationWs = refFinalRotationWs.Multiply( Quaternion(reticleRotationWs.GetRight(), m_refRotPitchWs) );
	}

	m_referenceTranform.SetQuaternion( refFinalRotationWs );

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );

	TODO( "Restrict pitch as per m_minPitchDegrees & m_maxPitchDegrees" );
}
