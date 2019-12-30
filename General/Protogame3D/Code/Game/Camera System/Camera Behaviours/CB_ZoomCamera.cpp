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
	float const deltaYawDegrees = m_mouseSensitivity * mousePosDelta.x;
	m_refRotYawWs += deltaYawDegrees;

	// Pitch
	float const deltaPitchDegrees = m_mouseSensitivity * mousePosDelta.y;
	FloatRange allowedDeltaPitch = FloatRange( m_minPitchDegrees - m_refRotPitchWs, m_maxPitchDegrees - m_refRotPitchWs );
	if( allowedDeltaPitch.Includes( deltaPitchDegrees ) )
	 	m_refRotPitchWs += deltaPitchDegrees;
	
	Quaternion const yawRot		= Quaternion( Vector3::UP, m_refRotYawWs );
	Quaternion const pitchRot	= Quaternion( yawRot.GetRight(), m_refRotPitchWs );

	// Set final rotation
	if( LookAtCalcDoesNotHaveRoll() )
		m_referenceTranform.SetQuaternion( yawRot.Multiply( pitchRot ) );
	else
		DebugRender2DText( 0.f, Vector2( -850.f, 280.f ), 15.f, RGBA_ORANGE_COLOR, RGBA_ORANGE_COLOR, "!! ZoomCamera control disabled b/c reticle x-offset is non-zero !!" );
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
	Quaternion reticlePitchPlaneOrienatation = Quaternion::IDENTITY;

	float calcPitchRotWs = m_refRotPitchWs;
	float calcYawRotWs = m_refRotYawWs;
	
	// Make the reticle look at the target yaw
	{
		// Step 1: 
		// Make reference's forward look at the target                                                                    
		//                                                       //                                                   
		//   (forward) Y |                                       //          (up) Y |                                 
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
		calcYawRotWs = refLookAtTargetDeg;

		// Step 2:
		// If the camera is at reference position, make its reticle x-offset look at target
		Quaternion const rotReticleLookAtTargetDelta = Quaternion( Vector3::UP, -m_reticleYawDegreesWs );
		refFinalRotation = refFinalRotation.Multiply( rotReticleLookAtTargetDelta );

		// Step 3:
		// Consider the camera-offset from reference pos. Apply extra rotation to make the camera's reticle look at target
		float camOffsetLookAtTargetDeg = 0.f;
		{
			//                                               
			//           * target        a = extra rotation  
			//          .|\                  on reticle dir  
			//       .`  |a\                 (from ref pos)  
			//       \   |  \                                
			// reticle\  |   \                               
			//    dir  \a|    * cam     Yaw Plane            
			//          \| .`             (Yp)               
			//    (0,0)  * ref                               
			Vector3	const reticleRightDir	= Quaternion( Vector3::UP, m_reticleYawDegreesWs ).RotatePoint( Vector3::RIGHT );
			float	const refToCamProj		= Vector3::DotProduct( reticleRightDir, m_cameraOffset );
			float	const refToTargetLenYp	= targetYp.GetLength();
			float	const angleRadians		= asinf( refToCamProj / refToTargetLenYp );
			
			camOffsetLookAtTargetDeg = RadianToDegree( angleRadians );
		}
		Quaternion const rotCamOffsetLookAtTargetDelta = Quaternion( Vector3::UP, -camOffsetLookAtTargetDeg );
		refFinalRotation = refFinalRotation.Multiply( rotCamOffsetLookAtTargetDelta );
		calcYawRotWs += -camOffsetLookAtTargetDeg;

		// Note:
		// As of now, the reticle is already looking at the target as far as yaw is considered.
		// So, to maintain its yaw, we can APPLY PITCH ROTATION ON THE RIGHT DIRECTION OF FOLLOWING ORIENTATION!
		// P.S. : No matter how much reticle x-offset your ZoomCamera has, rotation along Right Dir of this orientation will maintain the reticle-yaw.
		reticlePitchPlaneOrienatation = Quaternion( Vector3::UP, refLookAtTargetDeg - camOffsetLookAtTargetDeg );
	}

	// Make the reticle look at the target pitch
	{
		// Step 1:
		// Make the reticle look at target pitch, as if the camera was at reference pos
		//                                                                                                              
		//                                                        //                                                    
		//      (up) Y |                                          //           (up) Y |                                 
		//             |     *  target                            //                  |                                 
		//   Reticle   |    /                                     //                  |    / Z (forward)                
		// Pitch Plane |   /             p = angle to look        //                  |   /                  LEFT HANDED
		//   (Rpp)     |  /                     at target         //    World Space   |  /                      SYSTEM  
		//             | / p                                      //       (Ws)       | /                               
		//             |/________________                         //                  |/________________                
		//       (0,0) *  ref pos       X (forward)               //          (0,0,0) * origin        X (right)         
		Vector3 const rppUpDir		= reticlePitchPlaneOrienatation.GetUp();
		Vector3 const rppRightDir	= reticlePitchPlaneOrienatation.GetRight();
		Vector3 const rppForwardDir	= reticlePitchPlaneOrienatation.GetFront();

		Vector2 const refPosRpp = Vector2::ZERO;
		Vector2 targetRpp;
		{
			targetRpp.x = Vector3::DotProduct( rppForwardDir, refToTargetDisp );
			targetRpp.y = Vector3::DotProduct( rppUpDir, refToTargetDisp );
		}

		float const reticleLookAtTargetDeg = atan2fDegree( targetRpp.y, targetRpp.x );
		Quaternion const rotReticleLookAtTarget = Quaternion( rppRightDir, -reticleLookAtTargetDeg );
		refFinalRotation = refFinalRotation.Multiply( rotReticleLookAtTarget );
		calcPitchRotWs = -reticleLookAtTargetDeg;

		// Step 2:
		// If the camera was on reference pos, make its reticle y-offset look at the target
		Quaternion const rotReticleLookAtTargetDelta = Quaternion( rppRightDir, -m_reticlePitchDegreesWs );
		refFinalRotation = refFinalRotation.Multiply( rotReticleLookAtTargetDelta );
		calcPitchRotWs += -m_reticlePitchDegreesWs;

		// Step 3:
		// Consider camera-offset from reference pos. Apply extra rotation so the camera's reticle looks at the target
		float camOffsetLookAtTargetDeg = 0.f;
		{
			//                                                             
			//                 camera *.                p = extra rotation 
			//                       /   ^ .                on reticle dir 
			//                      /        ^ .            (from ref pos) 
			//               (0,0) /___________(_^_                        
			//                ref *. ) p           * target                
			//    Reticle     pos    ^ .          /                        
			//  Pitch Plane              ^ .     /                         
			//    (Rpp)                      ^ ./                          
			Vector3	const reticleUpDir		= Quaternion( Vector3::RIGHT, m_reticlePitchDegreesWs ).RotatePoint( Vector3::UP );		// TODO: Do we want Vector3::RIGHT here? rppRightDir is not equal to refRightDir, won't it matter?
			float	const refToCamProj		= Vector3::DotProduct( reticleUpDir, m_cameraOffset );									// TODO: Double check if the reticleUpDir we're getting is the correct one for using here
			float	const refToTargetLenRpp	= targetRpp.GetLength();
			float	const pitchAngleRadians	= asinf( refToCamProj / refToTargetLenRpp );
			camOffsetLookAtTargetDeg = RadianToDegree( pitchAngleRadians );
		}
		Quaternion const rotCameraOffsetLookAtTarget = Quaternion( rppRightDir, camOffsetLookAtTargetDeg );
		refFinalRotation = refFinalRotation.Multiply( rotCameraOffsetLookAtTarget );
		calcPitchRotWs += camOffsetLookAtTargetDeg;
	}

	if( LookAtCalcDoesNotHaveRoll() )
	{
		m_refRotYawWs = calcYawRotWs;
		m_refRotPitchWs = calcPitchRotWs;
	}
	else
	{
		// Note: 
		// refFinalRotation will have roll in it, right?
		// You can't just get rid of that roll & only extract the yaw-pitch of ref rotation. 
		// (B/c the roll in refFinalRotation matters for the ZoomCamera to look at the target)
		//
		// Sadly it defeats the purpose for us. Because we're in a model where roll can not be applied.
		m_referenceTranform.SetQuaternion( refFinalRotation );
	}

	// Debug Render
	DebugRender2DText( 1.f, Vector2::ZERO, 20.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "Looked at the Target!" );
}

bool CB_ZoomCamera::LookAtCalcDoesNotHaveRoll() const
{
	// As long as the reticle x-offset it ZERO, the ReticlePitchPlane's Right Dir will match with the reference right dir.
	// Which means there won't be any roll in the ZoomCamera::LookAtTargetPosition() calculations.
	return (m_reticleOffset.x == 0.f);
}
