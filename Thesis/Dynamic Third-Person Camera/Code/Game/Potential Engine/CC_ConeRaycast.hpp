#pragma once
#include "Game/Potential Engine/CameraConstrain.hpp"

struct WeightedRaycasts_CR;

class CC_ConeRaycast : public CameraConstrain
{
public:
	 CC_ConeRaycast( char const *name, CameraManager &manager, uint8_t priority );
	~CC_ConeRaycast();

private:
	float m_curveHeight			= 50.f;
	float m_curvewidthFactor	= 800.f;

public:
	void Execute( CameraState &suggestedCameraState );

private:
	void  ConeRaycastFromPlayerTowardsCamera( Vector3 playerPos, Vector3 cameraPos, std::vector< WeightedRaycasts_CR > &outConeRaycasts );
	float AdjustDistanceFromAnchorBasedOnRaycastResult( float currDistFromPlayer, std::vector< WeightedRaycasts_CR > const &coneRaycastResults );

	void DebugDrawRaycastResults( std::vector<WeightedRaycasts_CR> const &raycasts ) const;

	void  ChangeCurveAccordingToInput();
	float WeightCurve( float x, float maxHeight, float width ) const;	// A variation of Witch of Angesi like curve..
};
