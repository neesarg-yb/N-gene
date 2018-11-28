#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Plane3.hpp"

class AABB3
{
public:
	 AABB3();
	 AABB3( Vector3 const &min, Vector3 const &max );
	 AABB3( Vector3 const &center, float xDim, float yDim, float zDim );
	 AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
	~AABB3();

public:
	Vector3 mins;
	Vector3 maxs;

public:
	void	TranslateBy( Vector3 translation );

public:
	Vector3	GetSize() const;
	Vector3 GetCenter() const;

	float	GetDistanceFromPoint( Vector3 const &point ) const;
	bool	IsPointInsideMe( Vector3 const &point ) const;
	Vector3 GetClosestPointInsideBounds( Vector3 const &from ) const;
	void	GetSixPlanes( Plane3 *planeArray ) const;
};
