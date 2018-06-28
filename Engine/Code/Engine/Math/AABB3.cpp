#include "AABB3.hpp"

AABB3::AABB3()
{
	mins = Vector3::ZERO;
	maxs = Vector3::ZERO;
}

AABB3::AABB3( Vector3 const &min, Vector3 const &max )
{
	this->mins = min;
	this->maxs = max;
}

AABB3::AABB3( Vector3 const &center, float xDim, float yDim, float zDim )
{
	Vector3 halfDim = Vector3( xDim, yDim, zDim ) * 0.5f;
	
	this->mins		= center - halfDim;
	this->maxs		= center + halfDim;
}

AABB3::~AABB3()
{

}

