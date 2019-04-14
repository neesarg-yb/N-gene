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
	bool	paused = DebugPausePhysics();
	Vector2	debugUpdateStrPos =  Vector2( 350.f, 430.f );
	DebugRender2DText( 0.f, debugUpdateStrPos, 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, Stringf( "Player Update : [U] %s(%s)", paused ? "" : " ", paused ? "DISABLED" : "ENABLED" ) );

	if( paused )
		return;

	// According to input
	SetWillpowerAndStrengths();

	Entity::Update();
}

void Player::Render() const
{
	Vector3	worldBoundsCenter = m_position + Vector3( 0.f, 0.f, m_size.z * 0.5f);
	AABB3	worldBounds		  = AABB3( worldBoundsCenter, m_size.x, m_size.y, m_size.z );

	MDebugUtils::RenderCubeWireframe( worldBounds, RGBA_BLUE_COLOR, true );
}

bool Player::DebugPausePhysics()
{
	if( g_theInput->WasKeyJustPressed( 'U' ) )
		m_updateEnabled = !m_updateEnabled;
	
	return (m_updateEnabled == false);
}

void Player::SetWillpowerAndStrengths()
{
	XboxController const &theController = g_theInput->m_controller[0];

	// Set the movement directions
	float	flyIntention		= 0.f;
	Vector2	movementIntentionXY	= theController.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;

	flyIntention += theController.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];		// Go Up
	flyIntention -= theController.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];	// Go Down

	m_willpowerIntention = Vector3( movementIntentionXY.x, movementIntentionXY.y, flyIntention );

	// Set the strengths
	m_willpowerStrength = 5.f;
	m_flyStrength = 10.f;
}
