#pragma once
#include "Player.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/World/World.hpp"
#include "Game/Debug/DebugRenderUtils.hpp"

Player::Player( Vector3 spawnPosition, World const *inTheWorld, Clock *parentClock )
	: Entity( parentClock )
	, m_world( inTheWorld )
{
	m_position = spawnPosition;
}

Player::~Player()
{

}

void Player::Update()
{
	SetWillpowerForceAccordingToInput();

	Entity::Update();

	m_willpowerForce = Vector3::ZERO;
}

void Player::Render() const
{
	Vector3	worldBoundsCenter = m_position + Vector3( 0.f, 0.f, m_size.z * 0.5f);
	AABB3	worldBounds		  = AABB3( worldBoundsCenter, m_size.x, m_size.y, m_size.z );

	if( m_world->GetCamera()->m_cameraMode != CAMERA_1ST_PERSON )
	{
		// Body
		MDebugUtils::RenderCubeWireframe( worldBounds, RGBA_BLUE_COLOR, true );

		// Velocity
		MDebugUtils::RenderLine( m_position, RGBA_WHITE_COLOR, m_position + m_velocity, RGBA_ORANGE_COLOR, true );

		// Sphere - Collider
		Sphere collider = GetCollider();
		MDebugUtils::RenderSphereWireframe( collider.center, collider.radius, RGBA_GREEN_COLOR, false );
	}
}

void Player::UpdateIsInAir()
{
	Vector3		 posJustBelow		= m_position - Vector3( 0.f, 0.f, 0.05f );
	BlockLocator justBelowBlockLoc	= m_world->GetBlockLocatorForWorldPosition( posJustBelow );
	
	m_isInAir = (justBelowBlockLoc.GetBlock().IsSolid() == false);
}

void Player::SetWillpowerForceAccordingToInput()
{
	// Return if the input just controls the camera
	if( m_world->GetCamera()->m_cameraMode == CAMERA_DETATCHED && m_inputControlsPlayer == false )
		return;

	// Update this variables according to the input
	Vector2	mouseScreenDelta = g_theInput->GetMouseDelta();
	float	forwardMovement = 0.f;
	float	leftMovement = 0.f;
	float	upMovement = 0.f;
	bool	tryJump = false;

	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= 1.f;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= 1.f;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= 1.f;
	if( g_theInput->WasKeyJustPressed( SPACE ) )
		tryJump = true;

	// Movement Direction
	float	const camYaw	 = m_world->GetCamera()->m_yawDegreesAboutZ;
	Vector3 const forwardDir = Vector3( CosDegree(camYaw), SinDegree(camYaw), 0.f );
	Vector3 const leftDir	 = Vector3( forwardDir.y * -1.f, forwardDir.x, 0.f );
	Vector3 const upDir		 = Vector3( 0.f, 0.f, 1.f );

	Vector2	xyMovementIntention	= (forwardDir.IgnoreZ() * forwardMovement) + (leftDir.IgnoreZ() * leftMovement);
	float	flyMovmentIntention	= upMovement;

	// Set Willpower Forces
	AddWillpowerForceXY( xyMovementIntention * m_xyMovementStrength );
	AddWillPowerForceZ ( flyMovmentIntention * m_flyMovmentStrength );

	// Jump Impulse
	if( m_physicsMode == PHYSICS_WALK && m_isInAir == false && tryJump )
		AddWillPowerForceZ( 1500.f );
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

void Player::AddWillpowerForceXY( Vector2 const &movementForce )
{
	m_willpowerForce.x += movementForce.x;
	m_willpowerForce.y += movementForce.y;
}

void Player::AddWillPowerForceZ( float const flyForce )
{
	m_willpowerForce.z += flyForce;
}
