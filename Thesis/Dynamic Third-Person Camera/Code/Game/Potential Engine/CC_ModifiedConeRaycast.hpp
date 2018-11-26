#pragma once
#include <functional>
#include "Engine/Core/Ray3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Game/Potential Engine/CameraConstrain.hpp"


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
	float				m_curveHeight			= 50.f;
	float				m_curvewidthFactor		= 800.f;

	// Sphere raycast properties
	float				m_maxRotationDegrees	= 40.f;					// Allowed ROTATION from the camera's position (as in Polar Coordinates)
	int					m_numCircularLayers		= 5;
	std::vector< int >	m_numRaysInLayer		= { 5, 10, 15, 20, 25 };

private:
	float				m_fNumCircularLayers	= (float)m_numCircularLayers;

public:
	void	Execute( CameraState &suggestedCameraState );
	float	WeightCurve( float x, float maxHeight, float width ) const;	// A variation of Witch of Agnesi like curve..

private:
	void	DebugRenderSettingsDetails();
	void	ChangeSettingsAccordingToInput();							// Updates property variables for the cone-raycast
	void	IncrementMaxRotationDegrees( float deltaSeconds, float multiplier );
	void	IncrementNumCircularLayers ( float deltaSeconds, float multiplier );

	// Positions of all the points are in Cartesian Coordinates
	void	GeneratePointsOnSphere( std::vector< Vector3 > &outPoints, Vector3 referencePointOnSphere, Matrix44 const &cameraToWorldMatrix, float maxRotationDegrees, int numCircularLayers, std::vector< int > const &numPointsInLayer ) const;
};
