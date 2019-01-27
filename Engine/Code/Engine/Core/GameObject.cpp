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

	// Limit the speed - for XZ Plane
	Vector2 velocityXZ( m_velocity.x, m_velocity.z );
	float	velocityY	= m_velocity.y;
	float	speedXZ		= velocityXZ.GetLength();

	if( speedXZ <= 0.1f )
	{
		// Speed is too small, ignore it
		velocityXZ = Vector2::ZERO;
	}
	else if( speedXZ > m_maxSpeed )
	{
		// Speed is too large, limit it by maxSpeed
		velocityXZ = velocityXZ.GetNormalized() * m_maxSpeed;
	}

	m_velocity = Vector3( velocityXZ.x, velocityY, velocityXZ.y );

	currentPosition += m_velocity * deltaSeconds;			// Position from velocity
	m_transform.SetPosition( currentPosition );				// Set the position

	m_acceleration = Vector3::ZERO;							// Reset the acceleration for next frame
}

Vector3 GameObject::CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const
{
	// Defaults as not colliding
	UNUSED( radius );

	outIsColliding = false;
	return center;
}
