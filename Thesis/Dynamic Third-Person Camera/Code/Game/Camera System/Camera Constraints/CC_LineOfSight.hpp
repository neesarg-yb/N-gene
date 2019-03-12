#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"

class CC_LineOfSight : public CameraConstraint
{
public:
	 CC_LineOfSight( char const *name, CameraManager &manager, uint8_t priority );
	~CC_LineOfSight();

public:
	float m_radiusReduction = 0.4f;			// Distance by which the camera is move towards the player from impact point of the raycast

public:
	void Execute( CameraState &suggestedDestination );

private:
	void ProcessDebugInput();

};