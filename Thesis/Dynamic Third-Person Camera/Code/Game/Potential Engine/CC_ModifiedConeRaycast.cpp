#pragma once
#include "CC_ModifiedConeRaycast.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CameraManager.hpp"
#include "Game/Potential Engine/DebugCamera.hpp"
#include "Game/GameCommon.hpp"

CC_ModifiedConeRaycast::CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority )
	: CameraConstrain( name, manager, priority )
{

	m_curveCB = [ this ] ( float x ) { return WeightCurve( x, m_curveHeight, m_curvewidthFactor ); };
}

CC_ModifiedConeRaycast::~CC_ModifiedConeRaycast()
{

}

void CC_ModifiedConeRaycast::Execute( CameraState &suggestedCameraState )
{
	CameraContext context	= m_manager.GetCameraContext();
	Vector3 playerPosition	= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition	= suggestedCameraState.m_position;

	// Get points on sphere around the player, such that the camera is on its surface
	std::vector< Vector3 > raycastTargetPoints;
	GeneratePointsOnSphere( raycastTargetPoints, (cameraPosition - playerPosition), m_maxRotationDegrees, m_numCircularLayers, m_numRaysInLayer );

	// Prepare Rays and Weights from those points, for Raycast
	std::vector< WeightedRay_MCR > weightedRays;
	GenerateWeightedRays( weightedRays, playerPosition, raycastTargetPoints );

	// Do Raycasts!
	std::vector< RaycastResultWithWeight_MCR > raycastResults;
	GetRaycastResults( raycastResults, weightedRays );

	float reductionInRadius = CalculateReductionInRadius( raycastResults );

	// DEBUG RENDER
	for each (Vector3 debugPoint in raycastTargetPoints)
		DebugRenderSphere( 0.f, debugPoint + playerPosition, 0.1f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_XRAY );

	Matrix44 camModelMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();
	Vector3	 cameraUp	 = camModelMat.GetJColumn();
	Vector3	 cameraRight = camModelMat.GetIColumn();
	DebugRenderTag( 0.f, 0.35f, cameraPosition, cameraUp, cameraRight, RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", reductionInRadius) );
}

float CC_ModifiedConeRaycast::WeightCurve( float x, float maxHeight, float width ) const
{
	// Equation:
	//			        width                 
	//			y = ------------- * maxHeight 
	//			     x^2 + width             
	return (width / ((x*x) + width)) * maxHeight;
}

void CC_ModifiedConeRaycast::GeneratePointsOnSphere( std::vector<Vector3> &outPoints, Vector3 referencePoint, float maxRotationDegrees, int numCircularLayers, std::vector<int> const &numPointsInLayer ) const
{
	// Polar Coordinate of the reference point
	float rPolarRadius, rPolarRotation, rPolarAltitude;
	CartesianToPolar( referencePoint, rPolarRadius, rPolarRotation, rPolarAltitude );

	Vector3	const referencePolarPoint( rPolarRadius, rPolarRotation, rPolarAltitude ); 
	float	const rotationDegreesPerLayer = maxRotationDegrees / numCircularLayers;

	// For each circular layers
	for( int i = 0; i < numCircularLayers; i++ )
	{
		// Each layer is some degrees away from the reference point..
		Vector3 startPolarPoint		= referencePolarPoint;		
		startPolarPoint.y += rotationDegreesPerLayer * (i + 1);		// Each layer starts at different rotation

		Vector3 const startPoint = PolarToCartesian( startPolarPoint.x, startPolarPoint.y, startPolarPoint.z );

		// For each points in this layer
		int		totalPoints		 = numPointsInLayer[i];
		float	thetaPerPoint	 = 360.f / totalPoints;
		Vector3	rotateAroundAxis = referencePoint.GetNormalized();
		for( int j = 0; j < totalPoints; j++ )
		{
			// Rotate the point and get the final point
			float theta = thetaPerPoint * j;

			Quaternion rotator( rotateAroundAxis, theta );
			Vector3 thisPointOnLayer = rotator.RotatePoint( startPoint );

			outPoints.push_back( thisPointOnLayer );
		}
	}
}

void CC_ModifiedConeRaycast::GenerateWeightedRays( std::vector< WeightedRay_MCR > &outWeightedRays, Vector3 const &playerPosition, std::vector< Vector3 > const &raycastEndPositions ) const
{
	for each (Vector3 endPoint in raycastEndPositions)
	{
		Vector3 polarCoord;
		Vector3 playerToEndPoint = endPoint - playerPosition;
		CartesianToPolar( playerToEndPoint, polarCoord.x, polarCoord.y, polarCoord.z );

		// Calculate weight
		float weightRotation = m_curveCB( polarCoord.y );
		float weightAltitude = m_curveCB( polarCoord.z );
		float finalWeight	 = weightRotation * weightAltitude;

		// Get Ray
		float	rayLength	= playerToEndPoint.NormalizeAndGetLength();
		Ray3	ray			= Ray3( playerPosition, playerToEndPoint );

		outWeightedRays.push_back( WeightedRay_MCR( finalWeight, ray, rayLength ) );
	}
}

void CC_ModifiedConeRaycast::GetRaycastResults( std::vector< RaycastResultWithWeight_MCR > &outRaycastResults, std::vector< WeightedRay_MCR > const &weightedRays ) const
{
	CameraContext	 contex		= m_manager.GetCameraContext();
	raycast_std_func raycastCB	= contex.raycastCallback;

	for each (WeightedRay_MCR raycastInfo in weightedRays)
	{
		RaycastResult thisResult = raycastCB( raycastInfo.ray.startPosition, raycastInfo.ray.direction, raycastInfo.length );

		outRaycastResults.push_back( RaycastResultWithWeight_MCR( raycastInfo.weight, thisResult ) );
	}
}

float CC_ModifiedConeRaycast::CalculateReductionInRadius( std::vector< RaycastResultWithWeight_MCR > const &raycastResults ) const
{
	UNUSED( raycastResults );

	return 0.f;
}
