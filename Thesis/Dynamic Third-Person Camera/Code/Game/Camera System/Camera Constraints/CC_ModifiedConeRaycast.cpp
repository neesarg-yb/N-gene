#pragma once
#include "CC_ModifiedConeRaycast.hpp"
#include <limits>
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Complex.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Camera System/DebugCamera.hpp"

float s_raMultiplier				= 3.f;
float s_rotDegreesChangeSpeed		= 55.f;
float s_minRotChangePerFrameReqired	= 0.6f;

void ChaneRAMultiplier( Command &cmd )
{
	SetFromText( s_raMultiplier, cmd.GetNextString().c_str() );
}

void ChangeRotationDegreesChangeSpeed( Command &cmd )
{
	SetFromText( s_rotDegreesChangeSpeed, cmd.GetNextString().c_str() );
}

void ChangeMinRotationRequired( Command &cmd )
{
	SetFromText( s_minRotChangePerFrameReqired, cmd.GetNextString().c_str() );
}

WeightedTargetPoint_MCR::WeightedTargetPoint_MCR( Vector3 const &inTargetPoint, float inWeightRR )
	: weightRR( inWeightRR )
	, targetPoint( inTargetPoint )
{

}

WeightedTargetPoint_MCR::WeightedTargetPoint_MCR( Vector3 const &inTargetPoint, float inWeightRR, float inWeightAR )
	: weightRR( inWeightRR )
	, weightAR( inWeightAR )
	, targetPoint( inTargetPoint )
{

}

WeightedRaycastResult_MCR::WeightedRaycastResult_MCR( RaycastResult const &inResult, float inWeightRR, float inWeightAR )
	: result( inResult )
	, weightRR( inWeightRR )
	, weightAR( inWeightAR )
{

}

CC_ModifiedConeRaycast::CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority, CB_Follow *followBehavior )
	: CameraConstraint( name, manager, priority )
	, m_followBehavior( followBehavior )
{
	m_curveCB = [ this ] ( float x ) { return WeightCurve( x, m_curveHeight, m_curvewidthFactor ); };

	CommandRegister( "changeMultRA", ChaneRAMultiplier );
	CommandRegister( "changeRotSpeed", ChangeRotationDegreesChangeSpeed );
	CommandRegister( "changeMinRot", ChangeMinRotationRequired );
}

CC_ModifiedConeRaycast::~CC_ModifiedConeRaycast()
{

}

void CC_ModifiedConeRaycast::Execute( CameraState &suggestedCameraState )
{
	ChangeSettingsAccordingToInput();
	DebugRenderSettingsDetails();

	CameraContext context				= m_manager.GetCameraContext();
	Vector3 playerPosition				= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition				= suggestedCameraState.m_position;
	Vector3 projectedVelocity			= ProjectVectorOnPlane( context.cameraStateLastFrame.m_velocity, suggestedCameraState.GetTransformMatrix().GetKColumn() );
	Vector3 cameraPosRelativeToPlayer	= (cameraPosition - playerPosition);
	Matrix44 debugCamMatrix				= g_activeDebugCamera->GetCameraModelMatrix();

	// Get points on sphere around the player, such that the camera is on its surface
	std::vector< Vector3 >	targetPointsOnSphere = { cameraPosRelativeToPlayer };
	Matrix44				camToWorldMatrix	 = suggestedCameraState.GetTransformMatrix();
	GeneratePointsOnSphere( targetPointsOnSphere, cameraPosRelativeToPlayer, camToWorldMatrix, m_maxRotationDegrees, m_numCircularLayers, m_numRaysInLayer );

	// Give weights to the target points
	std::vector< WeightedTargetPoint_MCR > weightedTargetPoints;
	AssignWeightToTargetPoints( weightedTargetPoints, targetPointsOnSphere, cameraPosRelativeToPlayer, projectedVelocity );

	// DEBUG RENDER
	DebugRenderWeightedTargetPoints( weightedTargetPoints, suggestedCameraState, cameraPosRelativeToPlayer + projectedVelocity );

	for each (WeightedTargetPoint_MCR point in weightedTargetPoints)
	{
		Vector3 debugPointPos		= point.targetPoint + playerPosition;
		Rgba	sphereWeightColor	= GetColorFromWeight( point.weightRR );
		DebugRenderSphere( 0.f, debugPointPos, 0.1f, sphereWeightColor, sphereWeightColor, DEBUG_RENDER_XRAY );

		Vector3 rrWPos = Vector3( debugPointPos );
		Vector3 arWPos = Vector3( rrWPos.x, rrWPos.y - 0.10f, rrWPos.z );
		DebugRenderTag( 0.f, 0.08f, rrWPos, debugCamMatrix.GetJColumn(), debugCamMatrix.GetIColumn(), RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf( "rrW %.2f", point.weightRR ) );
		DebugRenderTag( 0.f, 0.08f, arWPos, debugCamMatrix.GetJColumn(), debugCamMatrix.GetIColumn(), RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf( "arW %.2f", point.weightAR ) );
	}
	
	// Perform Raycasts
	std::vector< WeightedRaycastResult_MCR > weightedRaycastResults;
	PerformRaycastOnTargetPoints( weightedRaycastResults, weightedTargetPoints, playerPosition );

	// Polar coordinate of camera, relative to the player position
	float cameraRadius, cameraRotation, cameraAltitude;
	TODO( "See if you can make this code local-offset compativble.. Checkout CB_Follow!" );
	CartesianToPolar( cameraPosRelativeToPlayer, cameraRadius, cameraRotation, cameraAltitude );

	// Calculate Reduction in Radius
	float reductionInRadius = CalculateRadiusReduction( weightedRaycastResults, cameraRadius );

	// Calculate rotation & altitude change
	float rotationChange = 0.f;
	float altitudeChange = 0.f;
	CalculateRotationAltitudeChange( weightedTargetPoints, weightedRaycastResults, suggestedCameraState, rotationChange, altitudeChange );

	// Set new position!
	cameraRadius	-= reductionInRadius;
	cameraRotation	+= rotationChange;
	cameraAltitude	+= altitudeChange;
	Vector3 newCamPos = PolarToCartesian( cameraRadius, cameraRotation, cameraAltitude ) + playerPosition;
	suggestedCameraState.m_position = newCamPos;
	
	// Inform Follow Behavior the new polar coordinates
	if( m_followBehavior != nullptr )
		m_followBehavior->SuggestChangedPolarCoordinate( cameraRadius, cameraRotation, cameraAltitude );

	// DEBUG RENDER
	DebugRenderTag( 0.f, 0.25f, newCamPos, debugCamMatrix.GetJColumn(), debugCamMatrix.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf( "-%.2f", reductionInRadius ) );

	DebugRenderBasis( 0.f, suggestedCameraState.GetTransformMatrix(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	DebugRenderBasis( 0.f, context.anchorGameObject->m_transform.GetWorldTransformMatrix(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
}

float CC_ModifiedConeRaycast::WeightCurve( float x, float maxHeight, float width ) const
{
	float shiftPeakBy = 1;
	x = x - shiftPeakBy;

	// Equation:
	//			        width                 
	//			y = ------------- * maxHeight 
	//			     x^2 + width             
	return (width / ((x*x) + width)) * maxHeight;
}

void CC_ModifiedConeRaycast::DebugRenderSettingsDetails()
{
	// Weights Curve 
	float stepSize = 0.01f;
	AABB2 graphBounds = AABB2( Vector2(-460.f, -385.f), 400.f, 100.f );
	FloatRange xRange = FloatRange( 0.f, 1.f );
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

	std::string maxRotDegreesStr = Stringf( "+[K] -[H], to change max rotation degrees: %0.1f", m_maxRotationDegrees );
	std::string numLayersStr	 = Stringf( "+[U] -[J], to change number circular layers: %d",  m_numCircularLayers );
	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 40.f ), 17.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, maxRotDegreesStr.c_str() );
	DebugRender2DText( 0.f, Vector2( graphBounds.mins.x, graphBounds.maxs.y + 60.f ), 17.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, numLayersStr.c_str() );
}

void CC_ModifiedConeRaycast::ChangeSettingsAccordingToInput()
{
	float hightChangeSpeed		= 1.f;
	float widthChangeSpeed		= 0.01f;
	float deltaSeconds			= (float)GetMasterClock()->GetFrameDeltaSeconds();

	if( g_theInput->IsKeyPressed( NUM_PAD_8 ) )
		m_curveHeight += hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_2 ) )
		m_curveHeight -= hightChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_6 ) )
		m_curvewidthFactor += widthChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( NUM_PAD_4 ) )
		m_curvewidthFactor -= widthChangeSpeed * deltaSeconds;
	if( g_theInput->IsKeyPressed( 'K' ) )
		IncrementMaxRotationDegrees( deltaSeconds,  1.f );			// Increment
	if( g_theInput->IsKeyPressed( 'H' ) )
		IncrementMaxRotationDegrees( deltaSeconds, -1.f );			// Decrement
	if( g_theInput->IsKeyPressed( 'U' ) )
		IncrementNumCircularLayers( deltaSeconds,  1.f );			// Increment
	if( g_theInput->IsKeyPressed( 'J' ) )
		IncrementNumCircularLayers( deltaSeconds, -1.f );			// Decrement

	m_curveHeight		= ClampFloat( m_curveHeight, 0.0001f, 10.f );
	m_curvewidthFactor	= ClampFloat( m_curvewidthFactor, 0.0001f, 10.f );

	m_curveCB = [ this ] ( float x ) { return WeightCurve( x, m_curveHeight, m_curvewidthFactor ); };
}

void CC_ModifiedConeRaycast::IncrementMaxRotationDegrees( float deltaSeconds, float multiplier )
{
	float incrementSpeed = 15.f;

	m_maxRotationDegrees += incrementSpeed * multiplier * deltaSeconds;
	m_maxRotationDegrees  = ClampFloat( m_maxRotationDegrees, 0.f, 180.f );
}

void CC_ModifiedConeRaycast::IncrementNumCircularLayers( float deltaSeconds, float multiplier )
{
	float incrementSpeed = 1.5f;

	m_fNumCircularLayers += incrementSpeed * multiplier * deltaSeconds;
	m_fNumCircularLayers  = ClampFloat( m_fNumCircularLayers, 0, 15 );
	m_numCircularLayers   = (int)floorf( m_fNumCircularLayers );

	// Add num of rays in newly added layer..
	while ( m_numRaysInLayer.size() < m_numCircularLayers )
		m_numRaysInLayer.push_back( m_numRaysInLayer.back() );
}

void CC_ModifiedConeRaycast::GeneratePointsOnSphere( std::vector<Vector3> &outPoints, Vector3 referencePointOnSphere, Matrix44 const &cameraToWorldMatrix, float maxRotationDegrees, int numCircularLayers, std::vector<int> const &numPointsInLayer ) const
{
	// Polar Coordinate of the reference point
	float rPolarRadius, rPolarRotation, rPolarAltitude;
	CartesianToPolar( referencePointOnSphere, rPolarRadius, rPolarRotation, rPolarAltitude );

	Vector3	 const referencePointPolar( rPolarRadius, 0.f, 90.f );	// We'll always generate points around this reference, and then move to to Camera's Space!
	float	 const rotationDegreesPerLayer	= maxRotationDegrees / numCircularLayers;
	Vector3	 const rotateAroundAxis			= Vector3::RIGHT;		// Point on each layer is a copy of startPoint, rotated around this axis

	Matrix44 const referenceToSphereMatrix	= Matrix44( Vector3::FRONT, Vector3::UP, Vector3::RIGHT * -1.f, PolarToCartesian( referencePointPolar.x, referencePointPolar.y, referencePointPolar.z ) );
	Matrix44 const cameraToSphereMatrix		= Matrix44( cameraToWorldMatrix.GetIColumn(), cameraToWorldMatrix.GetJColumn(), cameraToWorldMatrix.GetKColumn(), referencePointOnSphere );

	Matrix44 sphereToReferenceMatrix;
	bool invSuccsess = referenceToSphereMatrix.GetInverse( sphereToReferenceMatrix );
	GUARANTEE_RECOVERABLE( invSuccsess, "Error: Couldn't get inverse of the matrix, successfully!!" );

	// For each circular layers ( In Sphere's space )
	for( int i = 0; i < numCircularLayers; i++ )
	{
		// Each layer is some degrees away from the reference point..
		Vector3 startPointPolar = referencePointPolar;		
		startPointPolar.y += rotationDegreesPerLayer * (i + 1);		// Each layer starts at different rotation

		// We're gonna form a circle(or layer) from this point
		Vector3 const startPoint = PolarToCartesian( startPointPolar.x, startPointPolar.y, startPointPolar.z );

		// For each points in this layer ( In Sphere's space )
		int		totalPoints		 = numPointsInLayer[i];
		float	thetaPerPoint	 = 360.f / totalPoints;
		for( int j = 0; j < totalPoints; j++ )
		{
			// Rotate the point and get the final point
			float theta = thetaPerPoint * j;

			Quaternion rotator( rotateAroundAxis, theta );
			Vector3 thisPointOnLayer = rotator.RotatePoint( startPoint );

			// Get it in Reference Space
			Vector3 thisPointInRefSpace = sphereToReferenceMatrix.Multiply( thisPointOnLayer, 1.f );

			// Get it in Sphere Space, but treat the Camera's space as reference space now
			Vector3 thisPointAroundCamera = cameraToSphereMatrix.Multiply( thisPointInRefSpace, 1.f );

			outPoints.push_back( thisPointAroundCamera );
		}
	}
}

void CC_ModifiedConeRaycast::AssignWeightToTargetPoints( std::vector<WeightedTargetPoint_MCR> &outWeightedPoints, std::vector<Vector3> const &targetPoints, Vector3 const &cameraPosOnSphere, Vector3 const &projectedVelocity )
{
	Vector3 referenceVector		= cameraPosOnSphere + projectedVelocity;
	Vector3 referenceDirection	= referenceVector.GetNormalized();
	Vector3 cameraDirection		= cameraPosOnSphere.GetNormalized();
	
	int zeroDotProductCount = 0;
	FloatRange dotProductRange;	// Initialized as [ maxFloat, -maxFloat ]

	for each (Vector3 point in targetPoints)
	{
		// For radius reduction
		Vector3	pointDirection	= point.GetNormalized();
		float	dotProductRR	= Vector3::DotProduct( pointDirection, referenceDirection );
		float	weightRR		= m_curveCB( dotProductRR );

		// For altitude & rotation change
		float	dotProductAR	= Vector3::DotProduct( cameraDirection, pointDirection );
		
		// Because we don't want the target point on camera's position get any weight..
		if( AreEqualFloats( dotProductAR, 1.f, 2 ) )
		{
			dotProductAR = 0.f;
			zeroDotProductCount++;
		}

		outWeightedPoints.push_back( WeightedTargetPoint_MCR( point, weightRR, dotProductAR ) );

		// Because the center point will have zero weight, and we don't wanna change it
		if( dotProductAR != 0.f )
			dotProductRange.ExpandToInclude( dotProductAR );
	}

	float const rangeMapMinimum = 0.f;
	float const rangeMapMaximum = 1.f;
	for( int wpInd = 0; wpInd < outWeightedPoints.size(); wpInd++ )
	{
		WeightedTargetPoint_MCR &thisWeightedPoint = outWeightedPoints[ wpInd ];

		// Because the center point will have zero weight, and we don't wanna change it
		if( thisWeightedPoint.weightAR != 0.f )
			thisWeightedPoint.weightAR = RangeMapFloat( thisWeightedPoint.weightAR, dotProductRange.min, dotProductRange.max, rangeMapMinimum, rangeMapMaximum );
	}

	Vector2 zeroDotProductCountPos	= Vector2::ZERO;
	Vector2 dotProductRangePos		= Vector2( zeroDotProductCountPos.x, zeroDotProductCountPos.y + 20.f );
	Vector2 mappedFloatRangePos		= Vector2( dotProductRangePos.x, dotProductRangePos.y + 20.f );

	DebugRender2DText( 0.f, mappedFloatRangePos,	15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Mapped Dot Product Range      = [ %.2f, %.2f ]", rangeMapMinimum, rangeMapMaximum) );
	DebugRender2DText( 0.f, dotProductRangePos,		15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Dot Product Initial Range     = [ %.2f, %.2f ]", dotProductRange.min, dotProductRange.max) );
	DebugRender2DText( 0.f, zeroDotProductCountPos,	15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Count(targets weighting zero) = %d", zeroDotProductCount) );
}

void CC_ModifiedConeRaycast::PerformRaycastOnTargetPoints( std::vector< WeightedRaycastResult_MCR > &outRaycastResult, std::vector< WeightedTargetPoint_MCR > const &pointsOnSphere, Vector3 const &sphereCenter )
{
	CameraContext context = m_manager.GetCameraContext();

	for each (WeightedTargetPoint_MCR sPoint in pointsOnSphere)
	{
		Vector3 rayStartPos	 = sphereCenter;
		Vector3	rayDirection = sPoint.targetPoint;
		float	rayMaxLength = rayDirection.NormalizeAndGetLength();

		RaycastResult result = context.raycastCallback( rayStartPos, rayDirection, rayMaxLength );
		outRaycastResult.push_back( WeightedRaycastResult_MCR(result, sPoint.weightRR, sPoint.weightAR) );

		DebugRenderRaycast( 0.f, sphereCenter, result, 0.2f, RGBA_RED_COLOR, RGBA_GREEN_COLOR, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );
	}
}

float CC_ModifiedConeRaycast::CalculateRadiusReduction( std::vector< WeightedRaycastResult_MCR > const &raycastResults, float currentRadius )
{
	float sumWeights = 0.f;
	float sumWeightedReduction = 0.f;

	Matrix44 debugCamMat = g_activeDebugCamera->m_cameraTransform.GetWorldTransformMatrix();

	for each (WeightedRaycastResult_MCR raycast in raycastResults)
	{
		// If did not impact, don't do any reductions based on this ray
		if( raycast.result.didImpact == false )
		{
			sumWeightedReduction += 0.f;
			sumWeights += raycast.weightRR;

			// Debug the suggested-reduction
			Vector3 srPos = Vector3( raycast.result.impactPosition.x, raycast.result.impactPosition.y - 0.18f, raycast.result.impactPosition.z );
			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("srr %.1f", 0.f) );

			// Debug the actual-reduction
			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("frr %.2f", 0.f * raycast.weightRR) );

			continue;
		}
		else
		{
			float reductionFraction	 = ( 1.f - raycast.result.fractionTravelled );
			float suggestedReduction = currentRadius * reductionFraction;

			sumWeightedReduction += suggestedReduction * raycast.weightRR;
			sumWeights += raycast.weightRR;

			// Debug the suggested-reduction
			Vector3 srPos = Vector3( raycast.result.impactPosition.x, raycast.result.impactPosition.y - 0.18f, raycast.result.impactPosition.z );
			DebugRenderTag( 0.f, 0.03f, srPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, Stringf("srr %.1f", suggestedReduction) );

			// Debug the actual-reduction
			Vector3 arPos = Vector3( srPos.x, srPos.y - 0.05f, srPos.z );
			DebugRenderTag( 0.f, 0.03f, arPos, debugCamMat.GetJColumn(), debugCamMat.GetIColumn(), RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("frr %.2f", suggestedReduction * raycast.weightRR) );
		}
	}

	// If there were no hits from any ray casts
	if( sumWeights == 0.f )
		return 0.f;

	float  weightedAvgReduction	= sumWeightedReduction / sumWeights;
	return weightedAvgReduction;
}

void CC_ModifiedConeRaycast::CalculateRotationAltitudeChange( std::vector<WeightedTargetPoint_MCR> const &targetPoints, std::vector<WeightedRaycastResult_MCR> const &raycastResults, CameraState const &cameraState, float &rotationChange_out, float &altitudeChange_out )
{
	UNUSED( altitudeChange_out );

	// Contextual Info.
	Vector3 playerPosition		= m_manager.GetCameraContext().anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition		= cameraState.m_position;
	Vector3 cameraRelToPlayer	= cameraPosition - playerPosition;

	// Get Sphere to Camera Matrix
	Matrix44 sphereToCameraMatrix;
	Matrix44 cameraTransformMatrix = cameraState.GetTransformMatrix();	// Camera to World
	cameraTransformMatrix.SetTColumn( cameraRelToPlayer );				// Making it: Camera to Sphere
	bool inverseSuccess = cameraTransformMatrix.GetInverse( sphereToCameraMatrix );
	GUARANTEE_RECOVERABLE( inverseSuccess, "Error: Failed inverting the camera transform matrix!" );

	// Wighted average position change, in camera's space
	float	sumWeight		= 0.f;
	Vector2	sumImpactVector	= Vector2::ZERO;

	for( int tpInd = 0; tpInd < targetPoints.size(); tpInd++ )
	{
		WeightedTargetPoint_MCR const	&thisTargetPoint = targetPoints[tpInd];
		WeightedRaycastResult_MCR const	&thisRaycast	 = raycastResults[tpInd];

		if( thisRaycast.result.didImpact == false )
			continue;

		Vector3 pointOnSphere	= sphereToCameraMatrix.Multiply( thisTargetPoint.targetPoint, 1.f );
		Vector2 xyPointOnSphere	= Vector2( pointOnSphere.x, pointOnSphere.y );

		sumWeight		+= thisRaycast.weightAR;
		sumImpactVector	+= (xyPointOnSphere * thisRaycast.weightAR);
	}

	// If not impact at all..
	if( sumWeight == 0.f )
		return;

	Vector2 weightedAvgImpactVector = (sumImpactVector / sumWeight);
	Vector2 reactionVector			= weightedAvgImpactVector;
	
	Vector2 reactionVectorPos		= Vector2( 0.f, -20.f );
	Vector2 raMultPos				= Vector2( reactionVectorPos.x, reactionVectorPos.y - 20.f );
	Vector2 radiusReductionPos		= Vector2( raMultPos.x, raMultPos.y - 20.f );
	Vector2 altitudeReductionPos	= Vector2( radiusReductionPos.x, radiusReductionPos.y - 20.f );
	DebugRender2DText( 0.f, reactionVectorPos,		15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Reduction Vector   = (%.3f, %.3f)", reactionVector.x, reactionVector.y) );
	DebugRender2DText( 0.f, raMultPos,				15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, Stringf("RA Multiplier      =  %.3f", s_raMultiplier) );

	// Apply multiplier
	reactionVector.x *= s_raMultiplier;
	reactionVector.y *= s_raMultiplier;
	DebugRender2DText( 0.f, radiusReductionPos,		15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Rotation Reduction =  %.3f", reactionVector.x ) );
	DebugRender2DText( 0.f, altitudeReductionPos,	15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Altitude Reduction =  %.3f", reactionVector.y ) );

	// Turn towards target rotation
	float rotationChange = reactionVector.x;
	Complex currentRotChange( 0.f );
	Complex targetRotChange( rotationChange );

	float const deltaSeconds		= (float) GetMasterClock()->GetFrameDeltaSeconds();
	currentRotChange.TurnToward( targetRotChange, s_rotDegreesChangeSpeed * deltaSeconds );

	Vector2 rotSpeedPos = Vector2( altitudeReductionPos.x, altitudeReductionPos.y - 20.f );
	DebugRender2DText( 0.f, rotSpeedPos,			15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, Stringf("Rot Speed          =  %.1f (deg/sec)", s_rotDegreesChangeSpeed) );

	Vector2 rotChangePerFramePos = Vector2( rotSpeedPos.x, rotSpeedPos.y - 20.f );
	DebugRender2DText( 0.f, rotChangePerFramePos,	15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Rot Change / Frame =  %.3f", s_rotDegreesChangeSpeed * deltaSeconds) );

	rotationChange = currentRotChange.GetRotation();
	Vector2 rotChangeAppliedPos  = Vector2( rotChangePerFramePos.x, rotChangePerFramePos.y - 20.f );
	DebugRender2DText( 0.f, rotChangeAppliedPos,	15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("Applied Rot Change =  %.3f", rotationChange) );

	Vector2 minRotPerFramePos = Vector2( rotChangeAppliedPos.x, rotChangeAppliedPos.y - 20.f );
	DebugRender2DText( 0.f, minRotPerFramePos,		15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, Stringf("Min Rot Change Req =  %.1f / frame", s_minRotChangePerFrameReqired) );

	// If rotation change is greater than some threshold, apply it!
	Vector2 appliedNotAppliedPos = Vector2( minRotPerFramePos.x, minRotPerFramePos.y - 20.f );
	if( fabsf(rotationChange) > s_minRotChangePerFrameReqired )
	{
		rotationChange_out = rotationChange;
		DebugRender2DText( 0.f, appliedNotAppliedPos, 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("[APPLIED]") );
	}
	else
	{
		rotationChange_out = 0.f;
		DebugRender2DText( 0.f, appliedNotAppliedPos, 15.f, RGBA_RED_COLOR, RGBA_RED_COLOR, Stringf("[IGNORED]") );
	}
}

void CC_ModifiedConeRaycast::DebugRenderWeightedTargetPoints( std::vector< WeightedTargetPoint_MCR > const &targetPoints, CameraState const &cameraState, Vector3 const &projectedVelocity )
{
	Vector3 playerPosition		= m_manager.GetCameraContext().anchorGameObject->m_transform.GetWorldPosition();
	Vector3 cameraPosition		= cameraState.m_position;
	Vector3 cameraRelToPlayer	= cameraPosition - playerPosition;
	
	Matrix44 cameraTransformMatrix = cameraState.GetTransformMatrix();	// Camera to World
	cameraTransformMatrix.SetTColumn( cameraRelToPlayer );				// Making it: Camera to Sphere
	
	Matrix44 sphereToCameraMatrix;
	bool inverseSuccess = cameraTransformMatrix.GetInverse( sphereToCameraMatrix );
	GUARANTEE_RECOVERABLE( inverseSuccess, "Error: Failed inverting the camera transform matrix!" );
	
	AABB2					boundsPoints = AABB2( (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::min)(), (std::numeric_limits<float>::min)() );
	std::vector< Vector2 >	pointsToRender;
	std::vector< float >	weightsOfPoints;

	for each (WeightedTargetPoint_MCR point in targetPoints)
	{
		// Add vec2 point and its weight to the vector
		Vector3 pointInCameraSpace = sphereToCameraMatrix.Multiply( point.targetPoint, 1.f );
		Vector2 position2D( pointInCameraSpace.x, pointInCameraSpace.y );
		
		pointsToRender.push_back( position2D );
		weightsOfPoints.push_back( point.weightRR );

		// Update min & max bounds
		Vector2 &mins = boundsPoints.mins;
		Vector2 &maxs = boundsPoints.maxs;

		mins.x = ( position2D.x < mins.x ) ? position2D.x : mins.x;
		mins.y = ( position2D.y < mins.y ) ? position2D.y : mins.y;

		maxs.x = ( position2D.x > maxs.x ) ? position2D.x : maxs.x;
		maxs.y = ( position2D.y > maxs.y ) ? position2D.y : maxs.y;
	}

	// Render background
	AABB2 backgroundBounds = AABB2( Vector2(-710.f, -50.f), 150.f, 150.f );
	DebugRender2DQuad( 0.f, backgroundBounds, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR );

	// Bounds of canvas
	float radiusOfPoint = 5.f;
	AABB2 canvasBounds= AABB2(	backgroundBounds.mins.x + radiusOfPoint + 2.f, 
								backgroundBounds.mins.y + radiusOfPoint + 2.f, 
								backgroundBounds.maxs.x - radiusOfPoint - 2.f, 
								backgroundBounds.maxs.y - radiusOfPoint - 2.f	);
	
	// Render velocity-line
	Vector3 projVelInCameraSpace	= sphereToCameraMatrix.Multiply( projectedVelocity, 1.f );
	float	projVelScreenPositionX	= RangeMapFloat( projVelInCameraSpace.x, boundsPoints.mins.x, boundsPoints.maxs.x, canvasBounds.mins.x, canvasBounds.maxs.x );
	float	projVelScreenPositionY	= RangeMapFloat( projVelInCameraSpace.y, boundsPoints.mins.y, boundsPoints.maxs.y, canvasBounds.mins.y, canvasBounds.maxs.y );
	Vector2 projVelInCameraSpaceXY	= Vector2( projVelScreenPositionX, projVelScreenPositionY );
	DebugRender2DLine( 0.f, canvasBounds.GetCenter(), RGBA_BLUE_COLOR, projVelInCameraSpaceXY, RGBA_GREEN_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );

	// Render each points
	for( uint i = 0; i < pointsToRender.size(); i++ )
	{
		Vector2	&positionXY	= pointsToRender[i];
		float	&weight		= weightsOfPoints[i];

		float screenPositionX = RangeMapFloat( positionXY.x, boundsPoints.mins.x, boundsPoints.maxs.x, canvasBounds.mins.x, canvasBounds.maxs.x );
		float screenPositionY = RangeMapFloat( positionXY.y, boundsPoints.mins.y, boundsPoints.maxs.y, canvasBounds.mins.y, canvasBounds.maxs.y );
		
		Vector2	screenPosition( screenPositionX, screenPositionY );
		Rgba	weightColor = GetColorFromWeight( weight );

		DebugRender2DRound( 0.f, screenPosition, radiusOfPoint, weightColor, weightColor );
	}
}

Rgba CC_ModifiedConeRaycast::GetColorFromWeight( float weight ) const
{
	float	weightColorFraction = ClampFloat( RangeMapFloat( 1.f - weight, 0.f, 0.5f, 0.f, 1.f ), 0.f, 1.f );
	Rgba	sphereWeightColor	= Interpolate( RGBA_ORANGE_COLOR, RGBA_WHITE_COLOR, weightColorFraction );

	return sphereWeightColor;
}