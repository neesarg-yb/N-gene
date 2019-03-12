#pragma once
#include "CC_HandoverToShoulder.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"
#include "Game/Camera System/Camera Behaviours/CB_ShoulderView.hpp"

CC_HandoverToShoulder::CC_HandoverToShoulder( char const *name, CameraManager &manager, uint8_t priority, CB_ShoulderView &shoulderBehavior, CB_Follow &followBehavior )
	: CameraConstraint( name, manager, priority )
	, m_shoulderBehavior( shoulderBehavior )
	, m_followBehavior( followBehavior )
{

}

CC_HandoverToShoulder::~CC_HandoverToShoulder()
{

}

void CC_HandoverToShoulder::Execute( CameraState &suggestedCameraState )
{
	UNUSED( suggestedCameraState );

	PROFILE_SCOPE_FUNCTION();

	CameraContext	context			= m_manager.GetCameraContext();
	Vector3			playerPosition	= context.anchorGameObject->m_transform.GetWorldPosition();

	// Fire a raycast backwards
	Vector3			startPosition		= playerPosition;
	float			maxDistance			= m_followBehavior.m_distanceFromAnchor;
	Vector3			rayDirection		= suggestedCameraState.m_position - startPosition;
	float			startToCamDist		= rayDirection.NormalizeAndGetLength();

	if( AreEqualFloats(startToCamDist, 0.f, 4) )
		return;

	RaycastResult	hitResults			= context.raycastCallback( startPosition, rayDirection, maxDistance );
	float			distanceTravelled	= hitResults.fractionTravelled * maxDistance;
	
	// If too close from anchor
	if( distanceTravelled < m_thresholdDistance )
	{
		// Change to shoulder view
		m_manager.ChangeCameraBehaviourTo( m_shoulderBehavior.m_name, 0.5f );
		m_shoulderBehavior.SetupForIncomingHandover( m_followBehavior.m_rotationAroundAnchor, suggestedCameraState.m_position );
	}
}
