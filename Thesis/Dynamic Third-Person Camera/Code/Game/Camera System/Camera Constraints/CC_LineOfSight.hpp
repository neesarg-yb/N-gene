#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"
#include "Game/GameCommon.hpp"

class CC_LineOfSight : public CameraConstraint
{
public:
	 CC_LineOfSight( char const *name, CameraManager &manager, uint8_t priority );
	~CC_LineOfSight();

public:
	float m_radiusReduction = LOS_RADIUS_REDUCTION;			// Distance by which the camera is move towards the player from impact point of the raycast

public:
	void Execute( CameraState &suggestedDestination );

private:
	void ProcessDebugInput();

};