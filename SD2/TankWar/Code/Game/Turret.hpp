#pragma once
#include "Engine/Math/Transform.hpp"

class Tank;
class Bullet;
class Renderable;

class Turret
{
	friend Tank;

public:
	 Turret( Tank &parentTank );
	~Turret();

public:
	Transform	m_headTransform;		// Camera should be parented to this one
	Renderable*	m_barrelRenderable		= nullptr;

private:
	Tank&		m_parentTank;

public:
	void		LookAtPosition( Vector3 targetPosInWorldSpace, float deltaSeconds );
	Bullet*		CreateANewBullet();
};