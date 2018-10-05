#pragma once
#include "ContactPoint.hpp"

ContactPoint::ContactPoint( Vector3 const &position, Vector3 const &normal )
{
	this->position	= position;
	this->normal	= normal;
}
