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
	
	// Quaternion const yawRot			= Quaternion( Vector3::UP, m_refRotYawWs );
	// Quaternion const pitchRot		= Quaternion( yawRot.GetRight(), m_refRotPitchWs );
	// Quaternion const yawRticleRot	= Quaternion( Vector3::UP, m_refRotYawWs + m_reticleYawDegreesWs );
	// Quaternion const pitchRot		= Quaternion( yawRticleRot.GetRight(), m_refRotPitchWs );

	// Set final rotation
	// m_referenceTranform.SetQuaternion( m_referenceTranform.GetQuaternion().Multiply(pitchRot) );
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
	Vector3 const refToTargetDisp = targetWs - m_referenceTranform.GetWorldPosition();
	Quaternion refFinalRotation = Quaternion::IDENTITY;
	
	// Make the reticle look at the target yaw
	{
		// Step 1: 
		// Make reference's yaw look at the target                                                                    
		//                                                       //                                                   
		//   (forward) Y |                                       //         (up)  Y |                                 
		//               |                                       //                 |                                 
		//               |    * target                           //                 |    / Z (forward)                
		//               |   /               a = angle to look   //                 |   /                  LEFT HANDED
		//   Yaw Plane   |  /                      at target     //   World Space   |  /                      SYSTEM  
		//     (Yp)      |a/                                     //      (Ws)       | /                               
		//               |/_______________                       //                 |/________________                
		//         (0,0) * ref pos       X (right)               //         (0,0,0) * origin        X (right)         
		Vector2 const refPosYp = Vector2::ZERO;
		Vector2 targetYp;
		{
			targetYp.y = Vector3::DotProduct( Vector3::FRONT, refToTargetDisp );
			targetYp.x = Vector3::DotProduct( Vector3::RIGHT, refToTargetDisp );
		}

		float const refLookAtTargetDeg = atan2fDegree( targetYp.x, targetYp.y );
		Quaternion const rotRefLookAtTarget = Quaternion( Vector3::UP, refLookAtTargetDeg );
		refFinalRotation = refFinalRotation.Multiply( rotRefLookAtTarget );

		// Step 2:
		// If the camera is at reference position, make its reticle look at target
		Quaternion const rotReticleLookAtTargetDelta = Quaternion( Vector3::UP, -m_reticleYawDegreesWs );
		refFinalRotation = refFinalRotation.Multiply( rotReticleLookAtTargetDelta );

		// Step 3:
		// Consider the camera-offset from reference pos. Apply extra rotation to make the camera's reticle look at target
		float camOffsetLookAtTargetDeg = 0.f;
		{
			//                                               
			//           * target        a = extra rotation  
			//          .|\                  on reticle dir  
			//       . ` |a\                 (from ref pos)  
			//       \   |  \                                
			// reticle\  |   \                               
			//    dir  \a|    * cam     Yaw Plane            
			//          \| . `            (Yp)               
			//    (0,0)  * ref                               
			Vector3	const reticleRightDir	= Quaternion( Vector3::UP, m_reticleYawDegreesWs ).RotatePoint( Vector3::RIGHT );
			float	const refToCamProj		= Vector3::DotProduct( reticleRightDir, m_cameraOffset );
			float	const refToTargetLen	= targetYp.GetLength();
			float	const angleRadians		= asinf( refToCamProj / refToTargetLen );
			
			camOffsetLookAtTargetDeg = RadianToDegree( angleRadians );
		}
		Quaternion const rotCamOffsetLookAtTargetDelta = Quaternion( Vector3::UP, -camOffsetLookAtTargetDeg );
		refFinalRotation = refFinalRotation.Multiply( rotCamOffsetLookAtTargetDelta );
	}

	// Make the reticle look at the target pitch
	{
		// Note:
		// Reticle is looking at the target's yaw
		//  to maintain its yaw, we'll APPLY THE PITCH ON RETICLE DIR'S RIGHT AXES
	}


	m_referenceTranform.SetQuaternion( refFinalRotation );

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );

	TODO( "Restrict pitch as per m_minPitchDegrees & m_maxPitchDegrees" );
}
