#pragma once
#include "Player.hpp"
#include "Game/Projectile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/theGame.hpp"

Player::Player( Vector2 position, float orientation, std::string definitionName )
	:Actor( position, orientation, definitionName )
{
	m_showDebugInfo = false;
}

void Player::Update( float deltaSeconds )
{
	if( m_readyToDie == true )
		g_theGame->StartTransitionToState( DEFEAT );

	// Update the base class
	Actor::Update(deltaSeconds);

	// Handles XboxInput
	PerformBehaviourActions( deltaSeconds );

	// HeatMap											// TODO: Remove creation of heatMap from Update. Find a way to do it once when player is on the Map
	delete m_currentHeatMap;
	m_currentHeatMap = nullptr;

	// Initialize the heatMap
	const Map& starMap = *g_theGame->m_currentAdventure->m_currentMap;
	m_currentHeatMap = new HeatMap( starMap.m_dimension, (starMap.m_dimension.x * starMap.m_dimension.y) + 99.f );

	// Set player_position's heat to ZERO
	IntVector2 playerOnTile = IntVector2( m_position );
	m_currentHeatMap->SetHeat( 0.f, playerOnTile );

	// Update the heatMap
	UpdateTheHeatMap( m_currentHeatMap, *g_theGame->m_currentAdventure->m_currentMap );
}

void Player::Render()
{
	if( m_readyToDie == true )		// If dead, do not render
		return;

	Entity::Render();
}

void Player::PerformBehaviourActions( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Shoot a projectile on A Button
	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed )
		ShootAProjectile();

	// Move thePlayer around with XboxController
	float speedOfEntity			= m_definition->m_maxSpeed;
	float leftStickMagnitude	= g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].magnitude;
	float leftStickOrientation	= g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].orientationDegree;

	if( leftStickMagnitude > 0 )
		SetVelocityDirectionInDegree( leftStickMagnitude * speedOfEntity, leftStickOrientation );
	else
		SetSpeed( 0.f );
}

void Player::ResetHealth()
{
	m_currentHealth = static_cast<ActorDefinition*>(m_definition)->m_maxHealth;
	m_readyToDie	= false;
}

void Player::ShootAProjectile()
{
	float shootAtAngle = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].orientationDegree;
	Projectile* shootNew = new Projectile( m_position, shootAtAngle , "Fire", static_cast<ActorDefinition*>(m_definition)->m_faction );

	g_theGame->m_currentAdventure->m_currentMap->AddEntityToList( shootNew, ENTITY_TYPE_PROJECTILE );
}