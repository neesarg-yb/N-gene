#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"

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

public:
	void Execute( CameraState &suggestedCameraState );
};
