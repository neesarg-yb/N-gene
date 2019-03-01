#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"

class CB_Follow;
class CB_ShoulderView;

class CC_HandoverToShoulder : public CameraConstraint
{
public:
	 CC_HandoverToShoulder( char const *name, CameraManager &manager, uint8_t priority, CB_ShoulderView &shoulderBehavior, CB_Follow &followBehavior );
	~CC_HandoverToShoulder();

private:
	CB_Follow		&m_followBehavior;
	CB_ShoulderView	&m_shoulderBehavior;
	float			 m_thresholdDistance = 1.f;		// Minimum required dist( camera, player ) to switch to Shoulder Behavior

public:
	void Execute( CameraState &suggestedCameraState );
};
