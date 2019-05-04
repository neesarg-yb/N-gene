#pragma once
#include <functional>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Ray3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/CameraSystem/CameraConstraint.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"


//---------------------------------
// Structures
// 
struct WeightedTargetPoint_MCR
{
public:
	float	weightRR;					// Weight from curve, for Radius Reduction
	float	weightAR;					// For Altitude & Rotation Reduction
	Vector3	targetPoint;

public:
	WeightedTargetPoint_MCR( Vector3 const &inTargetPoint, float inWeightRR );
	WeightedTargetPoint_MCR( Vector3 const &inTargetPoint, float inWeightRR, float inWeightAR );
};

struct WeightedRaycastResult_MCR
{
public:
	RaycastResult	result;
	float			weightRR;			// Weight from curve, for Radius Reduction
	float			weightAR;			// For Altitude & Rotation Reduction

public:
	WeightedRaycastResult_MCR( RaycastResult const &inResult, float inWeightRR, float inWeightAR );
};


//---------------------------------
// Classes
// 
class CC_ModifiedConeRaycast : public CameraConstraint
{
public:
	 CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority, CB_Follow *followBehavior );
	~CC_ModifiedConeRaycast();

public:
	std::function<float (float x)> m_curveCB;

	// Curve's properties
	float				 m_curveHeight			= 1.00f;
	float				 m_curveWidthFactor		= 0.07f;

	// Sphere raycast properties
	float				 m_maxRotationDegrees	= 40.f;					// Allowed ROTATION from the camera's position (as in Polar Coordinates)
	int					 m_numCircularLayers	= 5;
	std::vector< int >	 m_numRaysInLayer		= { 5, 10, 15, 20, 25 };

	// Radius Reduction
	float				 m_radiusReductionMultiplier = 1.f;				// How much more the radius is reduced from the suggestion of cone raycasts

	// Raycast Reduction Weight Reaction
	float const			 m_velocityReactionFrac = 0.38f;				// How much weights reacts to the projected velocity
	float				 m_cachedAltitude		= 0.f;					// To cache the player's desired altitude, so we can roll back to it once the collision is avoided..

private:
	float				 m_fNumCircularLayers	= (float) m_numCircularLayers;
	CB_Follow			*m_followBehavior		= nullptr;
	bool				 m_isDebuggingForImpact	= true;
	eSphericalCoordinate m_debugSphericalCoord	= SPHERICAL_RADIUS;

public:
	void	Execute( CameraState &suggestedCameraState );
	float	WeightCurve( float x, float maxHeight, float width ) const;	// A variation of Witch of Agnesi like curve..

private:
	void	DebugRenderSettingsDetails();
	void	CycleNextSelectedSphericalCoord();
	void	DebugRenderSphericalCoordinates( CameraState updatedCamState, float radius, float rotation, float altitude ) const;
	void	ChangeSettingsAccordingToInput();							// Updates property variables for the cone-raycast
	void	IncrementMaxRotationDegrees( float deltaSeconds, float multiplier );
	void	IncrementNumCircularLayers ( float deltaSeconds, float multiplier );

	// Positions of all the points are in Cartesian Coordinates
	void	GeneratePointsOnSphere( std::vector< Vector3 > &outPoints, Vector3 referencePointOnSphere, Matrix44 const &cameraToWorldMatrix, float maxRotationDegrees, int numCircularLayers, std::vector< int > const &numPointsInLayer ) const;

	// Assigns weights by doing dot product between directions of target point & reference
	void	AssignWeightToTargetPoints( std::vector< WeightedTargetPoint_MCR > &outWeightedPoints, std::vector< Vector3 > const &targetPoints, Vector3 const &cameraPosOnSphere, Vector3 const &projectedVelocity );

	// Constructs Rays for raycasts
	void	PerformRaycastOnTargetPoints( std::vector< WeightedRaycastResult_MCR > &outRaycastResult, std::vector< WeightedTargetPoint_MCR > const &pointsOnSphere, Vector3 const &sphereCenter );

	// According to the wights of each RaycastResults, calculates reduction in current given radius
	float	CalculateRadiusReduction( std::vector< WeightedRaycastResult_MCR > const &raycastResults, float currentRadius );

	// According to which raycast did impact, it calculates the change in rotation and altitude of camera's polar coordinates, so the collision can be avoided
	void	CalculateRotationAltitudeChange( std::vector< WeightedTargetPoint_MCR > const &targetPoints, std::vector< WeightedRaycastResult_MCR > const &raycastResults, CameraState const &cameraState, float &rotationChange_out, float &altitudeChange_out );

	// Renders all the target points on a 2D canvas, for debugging their weights
	void	DebugRenderWeightedTargetPoints( std::vector< WeightedTargetPoint_MCR > const &targetPoints, std::vector< WeightedRaycastResult_MCR > const &raycastResults, CameraState const &cameraState, Vector3 const &projectedVelocity );
	Rgba	GetColorFromWeight( float weight ) const;
};
