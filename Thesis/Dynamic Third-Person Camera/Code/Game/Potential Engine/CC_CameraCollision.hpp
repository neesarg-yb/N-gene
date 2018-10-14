#pragma once
#include "Game/Potential Engine/CameraConstrain.hpp"

class CC_CameraCollision : public CameraConstrain
{
public:
	 CC_CameraCollision( char const *name, CameraManager &manager, uint8_t priority );
	~CC_CameraCollision();

public:
	void Execute( CameraState &suggestedCameraState );
};
