#pragma once
#include "Player.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Debug/DebugRenderUtils.hpp"

Player::Player( Vector3 spawnPosition, Clock *parentClock )
	: Entity( parentClock )
{
	m_position = spawnPosition;
}

Player::~Player()
{

}

void Player::Update()
{
	Entity::Update();
}

void Player::Render() const
{
	Vector3	worldBoundsCenter = m_position + Vector3( 0.f, 0.f, m_size.z * 0.5f);
	AABB3	worldBounds		  = AABB3( worldBoundsCenter, m_size.x, m_size.y, m_size.z );

	// Body
	MDebugUtils::RenderCubeWireframe( worldBounds, RGBA_BLUE_COLOR, true );
	
	// Velocity
	MDebugUtils::RenderLine( m_position, RGBA_WHITE_COLOR, m_position + m_velocity, RGBA_ORANGE_COLOR, true );

	// Sphere - Collider
	Sphere collider = GetCollider();
	MDebugUtils::RenderSphereWireframe( collider.center, collider.radius, RGBA_GREEN_COLOR, false );
}

Sphere Player::GetCollider() const
{
	// Assuming that the player's position is anchored at the bottom of the player's bounds
	constexpr float sphereRadius = 0.5f;
	Vector3 center = m_position + Vector3( 0.f, 0.f, sphereRadius );

	return Sphere( center, sphereRadius );
}

Vector3 Player::GetEyePosition() const
{
	return m_position + Vector3( 0.f, 0.f, m_size.z * 0.9f );
}

void Player::SetPositionFrom( Sphere const &fromCollider )
{
	// Assuming that the sphere's bottom touches the bottom of the player's bounds
	Vector3 playerPosition = fromCollider.center - Vector3( 0.f, 0.f, fromCollider.radius );
	m_position = playerPosition;
}

void Player::SetMovementWillpowerAndStrength( Vector2 const &xyMovementIntention, float strength )
{
	m_willpowerIntention.x = xyMovementIntention.x;
	m_willpowerIntention.y = xyMovementIntention.y;

	m_willpowerStrength = strength;
}

void Player::SetFlyWillpowerAndStrength( float const flyIntention, float strength )
{
	m_willpowerIntention.z = flyIntention;

	m_flyStrength = strength;
}
