#pragma once
#include "Ability_Move.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Battle.hpp"

Ability_Move::Ability_Move( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_Move::~Ability_Move()
{

}

bool Ability_Move::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
{
	// Camera & Input
	g_currentBattle->FocusCameraOnIndicatorTile	( deltaSeconds );
	g_currentBattle->RotateCameraWithRightStick	( deltaSeconds );
	g_currentBattle->ZoomTheCameraBasedOnTriggers( deltaSeconds );
	g_currentBattle->MoveIndicatorWithLeftStick	( deltaSeconds );

	// Wait for controller input
	XboxController& controller	= g_theInput->m_controller[0];
	if( controller.m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed )			// On selection of the destination tile
	{
		m_moveToPosition = g_currentBattle->m_indicatorTile->m_position;
		out_actionCancelled = false;
		return true;
	}
	else if( controller.m_xboxButtonStates[ XBOX_BUTTON_B ].keyJustPressed )	// On cancellation of the Action
	{
		g_currentBattle->MoveIndicatorToCurrentPlayerPosition();
		out_actionCancelled = true;
		return true;
	}

	out_actionCancelled = false;												// Player is still finalizing the Move
	return false;
}

bool Ability_Move::IsExecutingAction_Update( float deltaSeconds )
{
	// Perform the Move
	static float timeElasped = 0.f;
	timeElasped += deltaSeconds;
	float lerpFraction = timeElasped / m_totalLerpTime;

	Vector3 lerpPos			= Interpolate( m_hostActor.GetPosition(), m_moveToPosition, lerpFraction );
	m_hostActor.SetPosition( lerpPos );

	if( timeElasped >= m_totalLerpTime )
	{
		timeElasped = 0.f;
		return true;
	}
	else
		return false;
}