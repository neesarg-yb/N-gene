#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class GameObject
{
public:
			 GameObject() { };
	virtual ~GameObject() { };

public:
	Transform	 m_transform;
	Renderable	*m_renderable	= nullptr;

public:
	virtual void Update( float deltaSeconds ) = 0;
};