#pragma once
#include "Game/Potential Engine/Camera System/CameraConstraint.hpp"

class CC_CameraCollision : public CameraConstraint
{
public:
	 CC_CameraCollision( char const *name, CameraManager &manager, uint8_t priority );
	~CC_CameraCollision();

public:
	void Execute( CameraState &suggestedCameraState );
};
