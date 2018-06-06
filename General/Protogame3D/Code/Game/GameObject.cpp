#pragma once
#include "GameObject.hpp"

GameObject::GameObject( Transform const &transform )
	: m_transform( transform )
{

}

GameObject::GameObject(  Vector3 const &position, Vector3 const &rotation /* = Vector3::ZERO */, Vector3 const &scale /* = Vector3::ONE_ALL */ )
	: m_transform( position, rotation, scale )
{

}