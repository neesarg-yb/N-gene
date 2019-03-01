#pragma once
#include "CC_HandoverToShoulder.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
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
	GameObject const *anchor = m_manager.GetCameraContext().anchorGameObject;
	Vector3 anchorWorldPos	 = anchor->m_transform.GetWorldPosition();
	float distanceFromAnchor = (anchorWorldPos - suggestedCameraState.m_position).GetLength();

	// If too close from anchor
	if( distanceFromAnchor < 1.f )
	{
		// Change to shoulder view
		m_manager.ChangeCameraBehaviourTo( m_shoulderBehavior.m_name, 0.2f );
		m_shoulderBehavior.SetupForIncomingHandover( m_followBehavior.m_rotationAroundAnchor, true );
	}
}
