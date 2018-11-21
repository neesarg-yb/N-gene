#pragma once
#include <functional>
#include "Engine/Core/Ray3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Game/Potential Engine/CameraConstrain.hpp"


//---------------------------------
// Structures
// 
struct WeightedRay_MCR
{
public:
	Ray3	ray;
	float	weight;
	float	length;

public:
	WeightedRay_MCR( float inWeight, Ray3 const &inRay, float inLength )
		: weight( inWeight )
		, ray( inRay )
		, length( inLength ) { }
};

struct RaycastResultWithWeight_MCR
{
public:
	float			weight;
	RaycastResult	raycastResult;

public:
	RaycastResultWithWeight_MCR( float inWeight, RaycastResult const &inRaycastResult )
		: weight( inWeight )
		, raycastResult( inRaycastResult ) { }
};


//---------------------------------
// Classes
// 
class CC_ModifiedConeRaycast : public CameraConstrain
{
public:
	 CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority );
	~CC_ModifiedConeRaycast();

public:
	std::function<float (float x)> m_curveCB;

	// Curve's properties
	float				m_curveHeight		 = 50.f;
	float				m_curvewidthFactor	 = 800.f;

	// Sphere raycast properties
	float				m_maxRotationDegrees = 40.f;		// Allowed ROTATION from the camera's position (as in Polar Coordinates)
	int					m_numCircularLayers	 = 5;
	std::vector< int >	m_numRaysInLayer	 = { 5, 10, 15, 20, 25 };


public:
	void	Execute( CameraState &suggestedCameraState );
	float	WeightCurve( float x, float maxHeight, float width ) const;	// A variation of Witch of Agnesi like curve..

private:
	// Positions of all the points are in Cartesian Coordinates
	void GeneratePointsOnSphere( std::vector< Vector3 > &outPoints, Vector3 referencePoint, float maxRotationDegrees, int numCircularLayers, std::vector< int > const &numPointsInLayer ) const;

	// Generate rays for the raycast
	void GenerateWeightedRays( std::vector< WeightedRay_MCR > &outWeightedRays, Vector3 const &cameraPosition, Vector3 const &playerPosition, std::vector< Vector3 > const &endPointsRelativeToPlayer ) const;

	// Do the Raycasts
	void GetRaycastResults( std::vector< RaycastResultWithWeight_MCR > &outRaycastResults, std::vector< WeightedRay_MCR > const &weightedRays ) const;
	
	float CalculateReductionInRadius( float currentDistFromPlayer, std::vector< RaycastResultWithWeight_MCR > const &raycastResults ) const;
};
