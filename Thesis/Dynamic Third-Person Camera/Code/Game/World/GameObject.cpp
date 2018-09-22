#pragma once
#include "GameObject.hpp"

GameObject::GameObject()
{

}

GameObject::GameObject( Vector3 position, Vector3 rotation, Vector3 scale )
	: m_transform( position, rotation, scale )
{

}

GameObject::~GameObject()
{
	if( m_renderable != nullptr )
	{
		delete m_renderable;
		m_renderable = nullptr;
	}
}

void GameObject::Update( float deltaSeconds )
{
	// Update the position according to the velocity
	Vector3 currentPosition = m_transform.GetPosition();
	
	m_velocity += m_acceleration * deltaSeconds;			// Velocity from acceleration

	currentPosition += m_velocity * deltaSeconds;			// Position from velocity
	m_transform.SetPosition( currentPosition );				// Set the position

	m_acceleration = Vector3::ZERO;							// Reset the acceleration for next frame
}
