#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"
#include "Game/GameCommon.hpp"

class CB_Follow;
class CB_ShoulderView;

class CC_HandoverToFollow : public CameraConstraint
{
public:
	 CC_HandoverToFollow( char const *name, CameraManager &manager, uint8_t priority, CB_ShoulderView &shoulderBehavior, CB_Follow &followBehavior );
	~CC_HandoverToFollow();

private:
	CB_Follow		&m_followBehavior;
	CB_ShoulderView	&m_shoulderBehavior;
	float			 m_thresholdDistance = 1.3f + LOS_RADIUS_REDUCTION;	// Minimum required dist( camera, player ) to switch to Follow Behavior

public:
	void Execute( CameraState &suggestedCameraState );
};
