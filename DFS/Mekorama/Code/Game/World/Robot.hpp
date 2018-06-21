#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/IntVector3.hpp"

class Robot: public GameObject
{
public:
	 Robot( Vector3 const &worldPosition );
	~Robot();

public:
	Vector2		m_facingDirectionXZ	= Vector2( 0.f, 1.f );

public:
	void Update( float deltaSeconds );
	void ObjectSelected();
};