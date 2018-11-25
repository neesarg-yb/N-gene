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
	CameraContext context				= m_manager.GetCameraContext();
	Vector3 playerPosition				= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition				= suggestedCameraState.m_position;
	Vector3 cameraPosRelativeToPlayer	= (cameraPosition - playerPosition);

	// Get points on sphere around the player, such that the camera is on its surface
	std::vector< Vector3 > targetPointsOnSphere = { cameraPosRelativeToPlayer };
	GeneratePointsOnSphere( targetPointsOnSphere, cameraPosRelativeToPlayer, m_maxRotationDegrees, m_numCircularLayers, m_numRaysInLayer );

	// Prepare Rays and Weights from those points, for Raycast
	std::vector< WeightedRay_MCR > weightedRays;
	Vector3 maxWeightDirection = cameraPosRelativeToPlayer.GetNormalized();
	GenerateWeightedRays( weightedRays, maxWeightDirection, playerPosition, targetPointsOnSphere );

	// Do Raycasts!
	std::vector< RaycastResultWithWeight_MCR > raycastResults;
	GetRaycastResults( raycastResults, weightedRays );

	float distFromPlayer	= cameraPosRelativeToPlayer.GetLength();
	float reductionInRadius = CalculateReductionInRadius( distFromPlayer, raycastResults );

	// Move Camera forward if needed
	float radius, rotation, altitude;
	CartesianToPolar( cameraPosRelativeToPlayer, radius, rotation, altitude );
	radius -= reductionInRadius;

	Vector3 newCamPos = PolarToCartesian( radius, rotation, altitude ) + playerPosition;
	suggestedCameraState.m_position = newCamPos;

	// DEBUG RENDER
	for each (Vector3 debugPoint in targetPointsOnSphere)
	{
		DebugRenderSphere( 0.f, debugPoint + playerPosition, 0.1f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_USE_DEPTH );
	}

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
		Vector3 startPolarPoint = referencePolarPoint;		
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

void CC_ModifiedConeRaycast::GenerateWeightedRays( std::vector< WeightedRay_MCR > &outWeightedRays, Vector3 maxWeightDirection, Vector3 const &playerPosition, std::vector< Vector3 > const &endPointsRelativeToPlayer ) const
{
	Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();
	maxWeightDirection.NormalizeAndGetLength();

	for each (Vector3 endPoint in endPointsRelativeToPlayer)
	{
		float	radius		= endPoint.NormalizeAndGetLength();
		float	finalWeight	= Vector3::DotProduct( maxWeightDirection, endPoint );

		// Get Ray
		float	rayLength	= radius;
		Ray3	ray			= Ray3( playerPosition, endPoint );

		outWeightedRays.push_back( WeightedRay_MCR( finalWeight, ray, rayLength ) );

		// Debug the actual-reduction
		Vector3 weightTagPos = playerPosition + (endPoint * radius);
		DebugRenderTag( 0.f, 0.04f, weightTagPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("%.1f", finalWeight*100.f) );
	}
}

void CC_ModifiedConeRaycast::GetRaycastResults( std::vector< RaycastResultWithWeight_MCR > &outRaycastResults, std::vector< WeightedRay_MCR > const &weightedRays ) const
{
	CameraContext	 contex		= m_manager.GetCameraContext();
	raycast_std_func raycastCB	= contex.raycastCallback;

	for each (WeightedRay_MCR raycastInfo in weightedRays)
	{
		RaycastResult thisResult = raycastCB( raycastInfo.ray.startPosition, raycastInfo.ray.direction, raycastInfo.length );
		DebugRenderRaycast( 0.f, raycastInfo.ray.startPosition, thisResult, 0.1f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_KHAKI_COLOR, RGBA_YELLOW_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

		outRaycastResults.push_back( RaycastResultWithWeight_MCR( raycastInfo.weight, thisResult ) );
	}
}

float CC_ModifiedConeRaycast::CalculateReductionInRadius( float currentDistFromPlayer, std::vector< RaycastResultWithWeight_MCR > const &raycastResults ) const
{
	Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();

	// Do weighted average
	float sumWeights = 0.f;
	float sumWeightedReduction = 0.f;

	for each (RaycastResultWithWeight_MCR thisResult in raycastResults)
	{
		float const			 thisWeight = thisResult.weight;
		RaycastResult const &rResult	= thisResult.raycastResult;

		if( rResult.didImpact == false )
		{
			sumWeights += thisWeight;
			sumWeightedReduction += 0.f;

			// Debug the suggested-reduction
			Vector3 srPos = Vector3( rResult.impactPosition.x, rResult.impactPosition.y - 0.05f, rResult.impactPosition.z );
			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("%.1f", thisWeight) );

			// Debug the actual-reduction
			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", 0.f) );

			continue;
		}
		else
		{
			float reductionFraction		= ( 1.f - rResult.fractionTravelled );
			float suggestedReduction	= currentDistFromPlayer * reductionFraction;

			sumWeights += thisWeight;
			sumWeightedReduction += suggestedReduction * thisWeight;

			// Debug the suggested-reduction
			Vector3 srPos = Vector3( rResult.impactPosition.x, rResult.impactPosition.y - 0.05f, rResult.impactPosition.z );
			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("%.1f", thisWeight) );

			// Debug the actual-reduction
			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", suggestedReduction) );
		}
	}

	if( sumWeights == 0.f )
		return 0.f;

	float weightedAverage = sumWeightedReduction / sumWeights;
	return weightedAverage;
}
