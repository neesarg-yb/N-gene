#pragma once
#include <functional>
#include "Engine/Core/RaycastResult.hpp"
#include "Game/Potential Engine/Camera System/CameraConstrain.hpp"


//---------------------------------
// Structures
// 
struct WeightedRaycasts_CR
{
public:
	float weight = 0.f;
	RaycastResult ray;

public:
	WeightedRaycasts_CR( RaycastResult const &raycastResult, float raycastWeight )
		: ray( raycastResult )
		, weight( raycastWeight ) { }
};


//---------------------------------
// Classes
// 
class CC_ConeRaycast : public CameraConstrain
{
public:
	 CC_ConeRaycast( char const *name, CameraManager &manager, uint8_t priority );
	~CC_ConeRaycast();

private:
	std::function<float (float x)> m_curveCB;

	int		m_numSlices			= 15;
	float	m_coneAngle			= 45.f;
	float	m_curveHeight		= 50.f;
	float	m_curvewidthFactor	= 800.f;

	float	m_fNumSlices		= (float) m_numSlices;
	
public:
	void Execute( CameraState &suggestedCameraState );

private:
	void  ConeRaycastFromPlayerTowardsCamera( Vector3 playerPos, Vector3 cameraPos, std::vector< WeightedRaycasts_CR > *outConeRaycasts );			// Fills an array of outConeRaycasts[ NUM_RAY_DIRS ]; NUM_RAY_DIRS = 3
	float AdjustDistanceFromAnchorBasedOnRaycastResult( float currDistFromPlayer, std::vector< WeightedRaycasts_CR > const &coneRaycastResults );

	void  DebugDrawRaycastResults( std::vector<WeightedRaycasts_CR> const &raycasts );

	void  ChangeSettingsAccordingToInput();
	float WeightCurve( float x, float maxHeight, float width ) const;	// A variation of Witch of Angesi like curve..
};
