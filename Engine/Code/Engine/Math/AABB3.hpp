#pragma once
#include "Engine/Math/Vector3.hpp"

class AABB3
{
public:
	 AABB3();
	 AABB3( Vector3 const &min, Vector3 const &max );
	 AABB3( Vector3 const &center, float xDim, float yDim, float zDim );
	~AABB3();

public:
	Vector3 mins;
	Vector3 maxs;

public:
	bool IsPointInsideMe( Vector3 const &point ) const;
};