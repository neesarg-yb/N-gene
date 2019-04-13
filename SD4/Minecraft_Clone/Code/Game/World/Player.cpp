#pragma once
#include "Player.hpp"
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
	static bool playerUpdateEnabled = false;
	if( g_theInput->WasKeyJustPressed( 'U' ) )
		playerUpdateEnabled = !playerUpdateEnabled;

	if( !playerUpdateEnabled )
		return;

	Entity::Update();
}

void Player::Render() const
{
	Vector3	worldBoundsCenter = m_position + Vector3( 0.f, 0.f, m_size.z * 0.5f);
	AABB3	worldBounds		  = AABB3( worldBoundsCenter, m_size.x, m_size.y, m_size.z );

	MDebugUtils::RenderCubeWireframe( worldBounds, RGBA_BLUE_COLOR, true );
}
