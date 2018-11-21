#pragma once
#include <functional>
#include "Game/Potential Engine/CameraConstrain.hpp"

class CC_ModifiedConeRaycast : public CameraConstrain
{
public:
	 CC_ModifiedConeRaycast( char const *name, CameraManager &manager, uint8_t priority );
	~CC_ModifiedConeRaycast();

public:
	std::function<float (float x)> m_curveCB;

	float				m_maxRotationDegrees = 40.f;		// Allowed ROTATION from the camera's position (as in Polar Coordinates)
	int					m_numCircularLayers	 = 5;
	std::vector< int >	m_numRaysInLayer	 = { 5, 10, 15, 20, 25 };

public:
	void Execute( CameraState &suggestedCameraState );

private:
	// Positions of all the points are in Cartesian Coordinates
	void GeneratePointsOnSphere( std::vector< Vector3 > &outPoints, Vector3 referencePoint, float maxRotationDegrees, int numCircularLayers, std::vector< int > const &numPointsInLayer ) const;
};
