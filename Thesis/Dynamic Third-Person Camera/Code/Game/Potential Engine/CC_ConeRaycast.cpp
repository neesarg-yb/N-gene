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

enum eWeightedRaycastDir : int
{
	LEFT_RAYS = 0,
	RIGHT_RAYS,
	MIDDLE_RAYS,
	NUM_RAY_DIRS
};

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
	ChangeSettingsAccordingToInput();

	CameraContext	contex		= m_manager.GetCameraContext();
	Vector3			playerPos	= contex.anchorGameObject->m_transform.GetWorldPosition();
	Vector3			cameraPos	= suggestedCameraState.m_position;
	float distCameraToPlayer	= (playerPos - cameraPos).GetLength();

	std::vector< WeightedRaycasts_CR > coneRaycasts[ NUM_RAY_DIRS ];
	// Do Cone Raycast from anchor towards the camera
	ConeRaycastFromPlayerTowardsCamera( playerPos, cameraPos, coneRaycasts );

	for( int i = 0; i < NUM_RAY_DIRS; i++ )
		DebugDrawRaycastResults( coneRaycasts[i] );

	// Calculate suggested distance from anchor for each sides
	float distFromAnchor[ NUM_RAY_DIRS ];
	for( int i = 0; i < NUM_RAY_DIRS; i++ )
		distFromAnchor[i] = AdjustDistanceFromAnchorBasedOnRaycastResult( distCameraToPlayer, coneRaycasts[i] );
	
	// Take the lowest among all three directions
	float lowestDistFromAnchor = distFromAnchor[0];
	for( int i = 1; i < NUM_RAY_DIRS; i++ )
		lowestDistFromAnchor = (distFromAnchor[i] < lowestDistFromAnchor) ? distFromAnchor[i] : lowestDistFromAnchor;

	// Move Camera forward if needed
	float radius, rotation, altitude;
	CartesianToPolar( cameraPos - playerPos, radius, rotation, altitude );
	radius = lowestDistFromAnchor;

	Vector3 newCamPos = PolarToCartesian( radius, rotation, altitude ) + playerPos;
	suggestedCameraState.m_position = newCamPos;

	// Debug print the lowest-reduction
	Matrix44	debugCamMat		= g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();
	std::string	reductionStr	= Stringf("min( L:%.1f, M:%.1f, R:%.1f ) = %.1f",	distCameraToPlayer - distFromAnchor[LEFT_RAYS], 
																					distCameraToPlayer - distFromAnchor[MIDDLE_RAYS], 
																					distCameraToPlayer - distFromAnchor[RIGHT_RAYS], 
																					distCameraToPlayer - lowestDistFromAnchor);
	DebugRenderTag( 0.f, 0.25f, newCamPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, reductionStr.c_str() );
}

void CC_ConeRaycast::ConeRaycastFromPlayerTowardsCamera( Vector3 playerPos, Vector3 cameraPos, std::vector< WeightedRaycasts_CR > *outConeRaycasts )
{
	float const thetaPerSlice = m_coneAngle / m_numSlices;

	// Camera's Polar Coordinate
	PolarCoordinate currentCameraPolar;
	CartesianToPolar( cameraPos - playerPos, currentCameraPolar.radius, currentCameraPolar.rotation, currentCameraPolar.altitude );

	// Raycast
	CameraContext	 context			= m_manager.GetCameraContext();
	raycast_std_func raycastCB			= context.raycastCallback;
	Vector3 const	 startPos			= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3			 rayDirection		= (cameraPos - startPos);
	float			 maxDistance		= rayDirection.NormalizeAndGetLength();

	// Temp variables
	float			 raycastWeight		= m_curveCB( 0.f );		// At ZERO degrees
	PolarCoordinate	 raycastDestPoint	= currentCameraPolar;

	// Push the middle ray's result
	RaycastResult result = raycastCB( startPos, rayDirection, maxDistance );
	outConeRaycasts[ MIDDLE_RAYS ].push_back( WeightedRaycasts_CR(result, raycastWeight) );

	for( int i = 1; i <= m_numSlices; i++ )
	{
		// Polar Angles
		float posAngle = i * thetaPerSlice;
		float negAngle = posAngle * -1.f;

		// End points for the raycast
		PolarCoordinate posPolar( currentCameraPolar.radius, currentCameraPolar.rotation + posAngle, currentCameraPolar.altitude );
		PolarCoordinate negPolar( currentCameraPolar.radius, currentCameraPolar.rotation + negAngle, currentCameraPolar.altitude );
		
		Vector3 endPositivePoint = playerPos + PolarToCartesian( posPolar.radius, posPolar.rotation, posPolar.altitude );
		Vector3 endNegativePoint = playerPos + PolarToCartesian( negPolar.radius, negPolar.rotation, negPolar.altitude );
		
		// respective weights
		float posWeight = m_curveCB( posAngle );
		float negWeight = m_curveCB( negAngle );

		// Pos angles i.e. RIGHT from Anchor
		rayDirection = (endPositivePoint - startPos);
		maxDistance  = rayDirection.NormalizeAndGetLength();
		result		 = raycastCB( startPos, rayDirection, maxDistance );
		outConeRaycasts[ RIGHT_RAYS ].push_back( WeightedRaycasts_CR(result, posWeight) );

		// Neg angles i.e. LEFT from Anchor
		rayDirection = (endNegativePoint - startPos);
		maxDistance  = rayDirection.NormalizeAndGetLength();
		result		 = raycastCB( startPos, rayDirection, maxDistance );
		outConeRaycasts[ LEFT_RAYS ].push_back( WeightedRaycasts_CR(result, negWeight) );
	}
}

float CC_ConeRaycast::AdjustDistanceFromAnchorBasedOnRaycastResult( float currDistFromPlayer, std::vector< WeightedRaycasts_CR > const &coneRaycastResults )
{
	float sumWeights = 0.f;
	float sumWeightedReduction = 0.f;

	Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();

	for each (WeightedRaycasts_CR raycastResult in coneRaycastResults)
	{
		// If did not impact, don't do any reductions based on this ray
		if( raycastResult.ray.didImpact == false )
		{
 			sumWeightedReduction += 0.f;
 			sumWeights += raycastResult.weight;
 
 			// Debug the suggested-reduction
 			Vector3 srPos = Vector3( raycastResult.ray.impactPosition.x, raycastResult.ray.impactPosition.y - 0.05f, raycastResult.ray.impactPosition.z );
 			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("%.1f", 0.f) );
 
 			// Debug the actual-reduction
 			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
 			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("%.2f", 0.f * raycastResult.weight) );

			continue;
		}
		else
		{
			float reductionFraction		= ( 1.f - raycastResult.ray.fractionTravelled );
			float suggestedReduction	= currDistFromPlayer * reductionFraction;

			sumWeightedReduction += suggestedReduction * raycastResult.weight;
			sumWeights += raycastResult.weight;

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

	float weightedAvgReduction	= sumWeightedReduction / sumWeights;
	float suggestedDisatance	= currDistFromPlayer - weightedAvgReduction;

	return suggestedDisatance;
}

void CC_ConeRaycast::DebugDrawRaycastResults( std::vector<WeightedRaycasts_CR> const &raycasts )
{
	CameraContext contex = m_manager.GetCameraContext();
	Vector3 playerPos	 = contex.anchorGameObject->m_transform.GetWorldPosition();

	for each (WeightedRaycasts_CR raycastResult in raycasts)
	{
		// Raycast
		DebugRenderRaycast( 0.f, playerPos, raycastResult.ray, 0.f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

		// Weights
		Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();
		DebugRenderTag( 0.f, 0.03f, raycastResult.ray.impactPosition, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("%.2f", raycastResult.weight) );
	}

	// Weights Curve 
	float stepSize  = 2.5f;
	AABB2 graphBounds = AABB2( Vector2(-460.f, -385.f), 400.f, 100.f );
	FloatRange xRange = FloatRange( -100.f, 100.f );
	FloatRange yRange = DebugRenderXYCurve( 0.f, graphBounds, m_curveCB, xRange, stepSize, RGBA_GREEN_COLOR, RGBA_BLACK_COLOR, RGBA_GRAY_COLOR );

	float centerXCoordinate		= (graphBounds.mins.x + graphBounds.maxs.x) * 0.5f;
	Vector2 leftBottomCorner	= Vector2(graphBounds.mins.x, graphBounds.mins.y);
	Vector2 rightBottomCorner	= Vector2(graphBounds.maxs.x, graphBounds.mins.y);
	Vector2 topCenter = Vector2( centerXCoordinate, graphBounds.maxs.y );
	Vector2 botCenter = Vector2( centerXCoordinate, graphBounds.mins.y );
	DebugRender2DText( 0.f, leftBottomCorner,	17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", xRange.min) );
	DebugRender2DText( 0.f, rightBottomCorner,	17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", xRange.max) );
	DebugRender2DText( 0.f, topCenter,			17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", yRange.max) );
	DebugRender2DText( 0.f, botCenter,			17.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%.1f", yRange.min) );

	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 20.f ), 17.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, "Use numpad's 46-28 to change width-height!" );
	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 40.f ), 17.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, "+[K] -[H], to change cone angle." );
	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 60.f ), 17.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, "+[U] -[J], to change number of slices." );
}

void CC_ConeRaycast::ChangeSettingsAccordingToInput()
{
	float hightChangeSpeed		= 5.f;
	float widthChangeSpeed		= 200.f;
	float numSlicesChangeSpeed	= 3.f;
	float coneAngleChangeSpeed	= 15.f;
	float deltaSeconds = (float) GetMasterClock()->GetFrameDeltaSeconds();

	if( g_theInput->IsKeyPressed( NUM_PAD_8 ) )
		m_curveHeight += hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_2 ) )
		m_curveHeight -= hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_6 ) )
		m_curvewidthFactor += widthChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_4 ) )
		m_curvewidthFactor -= widthChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'K' ) )
		m_coneAngle += coneAngleChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'H' ) )
		m_coneAngle -= coneAngleChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'U' ) )
		m_fNumSlices += numSlicesChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'J' ) )
		m_fNumSlices -= numSlicesChangeSpeed * deltaSeconds;
	
	m_coneAngle			= ClampFloat( m_coneAngle, 1.f, 180.f );
	m_fNumSlices		= ClampFloat( m_fNumSlices, 1.f, 100.f );
	m_curveHeight		= ClampFloat( m_curveHeight, 1.f, 500.f );
	m_curvewidthFactor	= ClampFloat( m_curvewidthFactor, 1.f, 2000.f );
	
	m_numSlices		= (int) floorf(m_fNumSlices);
	m_curveCB		= [ this ] ( float x ) { return WeightCurve( x, m_curveHeight, m_curvewidthFactor ); };
}

float CC_ConeRaycast::WeightCurve( float x, float maxHeight, float width ) const
{
	// Equation:
	//			        width                 
	//			y = ------------- * maxHeight 
	//			     x^2 + width             
	return (width / ((x*x) + width)) * maxHeight;
}
