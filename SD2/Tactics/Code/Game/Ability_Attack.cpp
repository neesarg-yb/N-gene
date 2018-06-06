#pragma once
#include "Ability_Attack.hpp"
#include "Game/Battle.hpp"

Ability_Attack::Ability_Attack( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_Attack::~Ability_Attack()
{

}

bool Ability_Attack::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
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
		m_attackToPosition = g_currentBattle->m_indicatorTile->m_position;
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

bool Ability_Attack::IsExecutingAction_Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	// Get range

	// Get tiles where he can attack

	return true;
}