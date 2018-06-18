#pragma once
#include "Engine/Renderer/Camera.hpp"

// Camera that pivots around a target.
class OrbitCamera : public Camera
{
public:
	 OrbitCamera( Vector3 targetPos );
	~OrbitCamera();

public:
	Vector3 m_target; 

	float m_radius		= 1.f;    // distance from target
	float m_rotation	= 0.f;    // rotation around Y
	float m_altitude	= 0.f;    // rotation toward up after previous rotation

public:
	void SetSphericalCoordinate( float rad, float rot, float alt ); 
	void IncrementInSphericalCoordinate( float rad, float rot, float alt );

private:
	Vector3 GetOrbitCameraPosition() const;
};