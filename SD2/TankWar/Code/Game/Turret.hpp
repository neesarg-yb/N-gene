#pragma once
#include "Engine/Math/Transform.hpp"

class Tank;
class Renderable;

class Turret
{
	friend Tank;

public:
	 Turret( Tank &parentTank );
	~Turret();

public:
	Transform	m_headTransform;		// Camera should be parented to this one
	Transform	m_barrelTransform;

	Renderable*	m_barrelRenderable		= nullptr;

private:
	Tank&		m_parentTank;
};