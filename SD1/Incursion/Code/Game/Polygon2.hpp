#pragma once

#include "Entity.hpp"
#include "Engine/Math/MathUtil.hpp"

class Polygon2
{
public:
	Vector2 center;
	float   radius;
	float   sides;
	float   orientationAngle;

	 Polygon2();
	 Polygon2( float centerX, float centerY, float radius, float sides );
	 Polygon2( const Vector2& center, float radius, float sides );
	~Polygon2();

private:

};
