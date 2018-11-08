#pragma once
#include "CC_ConeRaycast.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CameraContext.hpp"
#include "Game/Potential Engine/CameraManager.hpp"
#include "Game/Potential Engine/DebugCamera.hpp"

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
	ChangeCurveAccordingToInput();

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
	int		const numSlices		= 4;
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

		// float weight = (numSlices - i) * (1.f / numSlices);
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
	float sumCount = 0.f;

	Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();

	for each (WeightedRaycasts_CR raycastResult in coneRaycastResults)
	{
		// If did not impact, don't do any reductions based on this ray
		if( raycastResult.ray.didImpact == false )
		{
// 			sumWeightedReduction += 0.f;
// 			sumWeights += raycastResult.weight;
// 
// 			// Debug the suggested-reduction
// 			Vector3 srPos = Vector3( raycastResult.ray.impactPosition.x, raycastResult.ray.impactPosition.y - 0.05f, raycastResult.ray.impactPosition.z );
// 			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("%.1f", 0.f) );
// 
// 			// Debug the actual-reduction
// 			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
// 			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", 0.f * raycastResult.weight) );

			continue;
		}
		else
		{
			float reductionFraction		= ( 1.f - raycastResult.ray.fractionTravelled );
			float suggestedReduction	= currDistFromPlayer * reductionFraction;

			sumWeightedReduction += suggestedReduction * raycastResult.weight;
			sumWeights += raycastResult.weight;
			sumCount += 1.f;

			// Debug the suggested-reduction
			Vector3 srPos = Vector3( raycastResult.ray.impactPosition.x, raycastResult.ray.impactPosition.y - 0.05f, raycastResult.ray.impactPosition.z );
			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("%.1f", suggestedReduction) );

			// Debug the actual-reduction
			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", suggestedReduction * raycastResult.weight) );
		}
	}

	// If there were no hits from any ray casts
	if( sumWeights == 0.f )
		return currDistFromPlayer;

	float weightedAvgReduction	= sumWeightedReduction / sumCount/*sumWeights*/;
	float suggestedDisatance	= currDistFromPlayer - weightedAvgReduction;
	
	// Debug the suggested-reduction
	DebugRenderTag( 0.f, 0.25f, m_manager.GetCurrentCameraState().m_position, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.1f", weightedAvgReduction) );

	return suggestedDisatance;
}

void CC_ConeRaycast::DebugDrawRaycastResults( std::vector<WeightedRaycasts_CR> const &raycasts ) const
{
	CameraContext contex = m_manager.GetCameraContext();
	Vector3 playerPos	 = contex.anchorGameObject->m_transform.GetWorldPosition();

	for each (WeightedRaycasts_CR raycastResult in raycasts)
	{
		// Raycast
		DebugRenderRaycast( 0.f, playerPos, raycastResult.ray, 0.f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

		// Weights
		Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();
		DebugRenderTag( 0.f, 0.03f, raycastResult.ray.impactPosition, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("%.2f%%", raycastResult.weight * 100.f) );
	}

	// Weights Curve
	std::function< float (float x) > curveCB = [ this ] ( float x ) { return WeightCurve( x, m_curveHeight, m_curvewidthFactor ); };
	float stepSize  = 2.5f;
	
	AABB2 graphBounds = AABB2( Vector2(-460.f, -385.f), 400.f, 100.f );
	FloatRange xRange = FloatRange( -100.f, 100.f );
	FloatRange yRange = DebugRenderXYCurve( 0.f, graphBounds, curveCB, xRange, stepSize, RGBA_GREEN_COLOR, RGBA_BLACK_COLOR, RGBA_GRAY_COLOR );

	float centerXCoordinate = (graphBounds.mins.x + graphBounds.maxs.x) * 0.5f;
	Vector2 leftBottomCorner = Vector2(graphBounds.mins.x, graphBounds.mins.y);
	Vector2 rightBottomCorner = Vector2(graphBounds.maxs.x, graphBounds.mins.y);
	Vector2 topCenter = Vector2( centerXCoordinate, graphBounds.maxs.y );
	Vector2 botCenter = Vector2( centerXCoordinate, graphBounds.mins.y );
	DebugRender2DText( 0.f, leftBottomCorner,	17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", xRange.min) );
	DebugRender2DText( 0.f, rightBottomCorner,	17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", xRange.max) );
	DebugRender2DText( 0.f, topCenter,			17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", yRange.max) );
	DebugRender2DText( 0.f, botCenter,			17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", yRange.min) );

	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 20.f ), 17.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, "Use numpad's 46-28 to change width-height!" );
}

void CC_ConeRaycast::ChangeCurveAccordingToInput()
{
	float hightChangeSpeed = 5.f;
	float widthChangeSpeed = 200.f;
	float deltaSeconds = (float) GetMasterClock()->GetFrameDeltaSeconds();

	if( g_theInput->IsKeyPressed( NUM_PAD_8 ) )
		m_curveHeight += hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_2 ) )
		m_curveHeight -= hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_6 ) )
		m_curvewidthFactor += widthChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_4 ) )
		m_curvewidthFactor -= widthChangeSpeed * deltaSeconds;

	m_curveHeight		= ClampFloat( m_curveHeight, 1.f, 500.f );
	m_curvewidthFactor	= ClampFloat( m_curvewidthFactor, 1.f, 2000.f );
}

float CC_ConeRaycast::WeightCurve( float x, float maxHeight, float width ) const
{
	// Equation:
	//			        width                 
	//			y = ------------- * maxHeight 
	//			     x^2 + width             
	return (width / ((x*x) + width)) * maxHeight;
}
