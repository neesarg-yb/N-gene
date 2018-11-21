#pragma once
#include "CC_ModifiedConeRaycast.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CameraManager.hpp"

CC_ModifiedConeRaycast::CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority )
	: CameraConstrain( name, manager, priority )
{

}

CC_ModifiedConeRaycast::~CC_ModifiedConeRaycast()
{

}

void CC_ModifiedConeRaycast::Execute( CameraState &suggestedCameraState )
{
	CameraContext context	= m_manager.GetCameraContext();
	Vector3 playerPosition	= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition	= suggestedCameraState.m_position;

	std::vector< Vector3 > raycastTargetPoints;
	GeneratePointsOnSphere( raycastTargetPoints, (cameraPosition - playerPosition), m_maxRotationDegrees, m_numCircularLayers, m_numRaysInLayer );

	for each (Vector3 debugPoint in raycastTargetPoints)
		DebugRenderSphere( 0.f, debugPoint + playerPosition, 0.1f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, DEBUG_RENDER_XRAY );
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
