#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class GameObject
{
public:
			 GameObject( Transform const &transform );
			 GameObject( Vector3 const &position, Vector3 const &rotation = Vector3::ZERO, Vector3 const &scale = Vector3::ONE_ALL );
	virtual ~GameObject() { };

public:
	Transform	 m_transform;
	Renderable	*m_renderable	= nullptr;

public:
	virtual void Update( float deltaSeconds ) = 0;
	virtual void PreRender() = 0;
};