#pragma once
#include "Ray3.hpp"

Ray3::Ray3( Vector3 const &startPosition, Vector3 const &direction )
{
	this->startPosition	= startPosition;
	this->direction		= direction;
}

Vector3 Ray3::Evaluate( float t )
{
	return startPosition + ( direction * t );
}

