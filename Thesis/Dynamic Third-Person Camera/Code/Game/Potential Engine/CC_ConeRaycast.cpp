#pragma once
#include "CC_ConeRaycast.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CameraContext.hpp"
#include "Game/Potential Engine/CameraManager.hpp"

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

struct PolarCoordinate
{
public:
	float radius	= 0.f;
	float rotation	= 0.f;
	float altitude	= 0.f;
	
public:
	PolarCoordinate() { }
	PolarCoordinate( float radius, float rotation, float altitude )
	{
		this->radius	= radius;
		this->rotation	= rotation;
		this->altitude	= altitude;
	}
};

CC_ConeRaycast::CC_ConeRaycast( char const *name, CameraManager &manager, uint8_t priority )
	: CameraConstrain( name, manager, priority )
{

}

CC_ConeRaycast::~CC_ConeRaycast()
{

}

void CC_ConeRaycast::Execute( CameraState &suggestedCameraState )
{
	CameraContext	contex		= m_manager.GetCameraContext();
	Vector3			playerPos	= contex.anchorGameObject->m_transform.GetWorldPosition();
	Vector3			cameraPos	= suggestedCameraState.m_position;
	float distCameraToPlayer	= (playerPos - cameraPos).GetLength();

	std::vector< WeightedRaycasts_CR > coneRaycasts;
	// Do Cone Raycast from anchor towards the camera
	ConeRaycastFromPlayerTowardsCamera( playerPos, cameraPos, coneRaycasts );

	DebugDrawRaycastResults( coneRaycasts );

	// Calculate weight according to which RayCast hits surrounding
	float distFromAnchor = AdjustDistanceFromAnchorBasedOnRaycastResult( distCameraToPlayer, coneRaycasts );

	// Move Camera forward if needed
	float radius, rotation, altitude;
	CartesianToPolar( cameraPos - playerPos, radius, rotation, altitude );
	radius = distFromAnchor;

	Vector3 newCamPos = PolarToCartesian( radius, rotation, altitude ) + playerPos;
	suggestedCameraState.m_position = newCamPos;
}

void CC_ConeRaycast::ConeRaycastFromPlayerTowardsCamera( Vector3 playerPos, Vector3 cameraPos, std::vector< WeightedRaycasts_CR > &outConeRaycasts )
{
	int		const numSlices		= 2;
	float	const coneAngle		= 60.f;
	float	const thetaPerSlice = coneAngle / numSlices;

	PolarCoordinate currentCameraPolar;
	CartesianToPolar( cameraPos - playerPos, currentCameraPolar.radius, currentCameraPolar.rotation, currentCameraPolar.altitude );

	CameraContext	 context	= m_manager.GetCameraContext();
	raycast_std_func raycastCB	= context.raycastCallback;
	
	std::vector< PolarCoordinate >	raycastDestPoints;
	std::vector< float >			raycastWeights;

	raycastDestPoints.push_back( currentCameraPolar );
	raycastWeights.push_back( 1.f );

	for( int i = 1; i <= numSlices; i++ )
	{
		float posAngle = i * thetaPerSlice;
		float negAngle = posAngle * -1.f;

		PolarCoordinate posPolar( currentCameraPolar.radius, currentCameraPolar.rotation + posAngle, currentCameraPolar.altitude );
		PolarCoordinate negPolar( currentCameraPolar.radius, currentCameraPolar.rotation + negAngle, currentCameraPolar.altitude );

		raycastDestPoints.push_back( posPolar );
		raycastDestPoints.push_back( negPolar );

		float weight = pow(0.8f, i);
		raycastWeights.push_back( weight );
		raycastWeights.push_back( weight );
	}

	for( int i = 0; i < raycastDestPoints.size(); i++ )
	{
		float weight = raycastWeights[i];
		PolarCoordinate destPolar = raycastDestPoints[i];

		Vector3	startPos		= context.anchorGameObject->m_transform.GetWorldPosition();
		Vector3	rayDirection	= PolarToCartesian( destPolar.radius, destPolar.rotation, destPolar.altitude ).GetNormalized();
		float	maxDistance		= (cameraPos - playerPos).GetLength();

		RaycastResult result = raycastCB( startPos, rayDirection, maxDistance );
		outConeRaycasts.push_back( WeightedRaycasts_CR(result, weight) );
	}
}

float CC_ConeRaycast::AdjustDistanceFromAnchorBasedOnRaycastResult( float currDistFromPlayer, std::vector< WeightedRaycasts_CR > const &coneRaycastResults )
{
	float sumWeightedReduction = 0.f;
	float sumWeights = 0.f;

	for each (WeightedRaycasts_CR raycastResult in coneRaycastResults)
	{
		// If did not impact, don't do any reductions based on this ray
		if( raycastResult.ray.didImpact == false )
			continue;

		float reductionFraction		= ( 1.f - raycastResult.ray.fractionTravelled );
		float suggestedReduction	= currDistFromPlayer * reductionFraction;

		sumWeightedReduction += suggestedReduction * raycastResult.weight;
		sumWeights += raycastResult.weight;
	}

	// If there were no hits from any ray casts
	if( sumWeights == 0.f )
		return currDistFromPlayer;

	float weightedAvgReduction	= sumWeightedReduction / sumWeights;
	float suggestedDisatance	= currDistFromPlayer - weightedAvgReduction;

	return suggestedDisatance;
}

void CC_ConeRaycast::DebugDrawRaycastResults( std::vector<WeightedRaycasts_CR> const &raycasts ) const
{
	CameraContext contex = m_manager.GetCameraContext();
	Vector3 playerPos	 = contex.anchorGameObject->m_transform.GetWorldPosition();

	for each (WeightedRaycasts_CR raycastResult in raycasts)
		DebugRenderRaycast( 0.f, playerPos, raycastResult.ray, 0.5f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );
}

