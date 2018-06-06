#pragma once

class Vector3
{
public:
	static Vector3 ZERO;

public:
			  Vector3() {};
			  Vector3( const Vector3& copyFrom );			
	 explicit Vector3( float initialX, float initialY, float initialZ );		
			 ~Vector3() {};
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};