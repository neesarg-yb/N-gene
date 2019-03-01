#pragma once
#include "CC_HandoverToFollow.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"
#include "Game/Camera System/Camera Behaviours/CB_ShoulderView.hpp"

CC_HandoverToFollow::CC_HandoverToFollow( char const *name, CameraManager &manager, uint8_t priority, CB_ShoulderView &shoulderBehavior, CB_Follow &followBehavior )
	: CameraConstraint( name, manager, priority )
	, m_shoulderBehavior( shoulderBehavior )
	, m_followBehavior( followBehavior )
{

}

CC_HandoverToFollow::~CC_HandoverToFollow()
{

}

void CC_HandoverToFollow::Execute( CameraState &suggestedCameraState )
{
	UNUSED( suggestedCameraState );

	CameraContext	context			= m_manager.GetCameraContext();
	Vector3			playerPosition	= context.anchorGameObject->m_transform.GetWorldPosition();

	// Fire a raycast backwards
	Vector3			startPosition		= playerPosition;
	float			maxDistance			= m_followBehavior.m_distanceFromAnchor;
	float			altitudeFromTop		= 90.f - m_shoulderBehavior.m_localPitchOffset;
	Vector3			rayDirection		= PolarToCartesian( 1.f, m_shoulderBehavior.m_rotationAroundAnchor, altitudeFromTop );
	RaycastResult	hitResults			= context.raycastCallback( startPosition, rayDirection, maxDistance );
	float			distanceTravelled	= hitResults.fractionTravelled * maxDistance;

	DebugRenderRaycast( 0.f, startPosition, hitResults, 1.f, RGBA_PURPLE_COLOR, RGBA_WHITE_COLOR, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

	// Compare if hit distance is > threshold
	float thresholdToActivate = 1.5f;
	if( distanceTravelled > thresholdToActivate )
	{
		m_manager.ChangeCameraBehaviourTo( m_followBehavior.m_name, 0.2f );
		m_followBehavior.SetupForIncomingHandover( m_shoulderBehavior.m_rotationAroundAnchor, altitudeFromTop );
	}
}

