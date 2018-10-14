#pragma once
#include "Sphere.hpp"

Sphere::Sphere()
{

}

Sphere::Sphere( Vector3 const &centerPos, float radiusLength )
	: center( centerPos )
	, radius( radiusLength )
{

}

Sphere::~Sphere()
{

}

