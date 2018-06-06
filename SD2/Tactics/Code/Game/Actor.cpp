#pragma once
#include "Actor.hpp"

#include <functional>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Ability.hpp"
#include "Game/Battle.hpp"

int numActionsPerformedThisTurn = 0;

Actor::Actor( Map& currentMap, Vector3 position, ActorDefinition* actorDefinition )
	: m_position( position )
	, m_definition( actorDefinition )
	, m_currentHealth( m_definition->m_maxHealth )
	, m_currentMap( currentMap )
{
	// Setup the Animator
	m_animator = new Animator( m_definition->m_animationSet );
	m_animator->Play( "walk" );

	// Setup the UIMenu
	m_actionsMenu = new UIMenu( *g_theInput, *g_theRenderer );
	for( std::map< std::string, AbilityData* >::iterator it =  m_definition->m_dataOfAbilities.begin(); 
														 it != m_definition->m_dataOfAbilities.end(); 
														 it++ )
	{
		m_actionsMenu->AddNewMenuAction( MenuAction( it->first.c_str(), &callBack ) );
	}

	// Get the shaderProgram
	m_ignoreAlphaShaderProgram = g_theRenderer->CreateOrGetShaderProgram( "discard_alpha" );
}

Actor::~Actor()
{
	delete m_actionsMenu;
	delete m_animator;
}

void Actor::Update( float deltaSeconds )
{
	m_animator->Update( deltaSeconds );
}

void Actor::Render() const
{
	static Transform spriteTransform;

	// Hecky way of doing draw offset
	Vector3				drawPosition		= Renderer::GetDrawPositionUsingAnchorPoint( m_position, m_definition->m_anchorBoxSize, m_definition->m_anchorPoint );
	float				cameraOrienation	= -g_currentBattle->m_camera->m_rotation + 270.f;
	SpriteWithScale*	spriteToRender		= m_animator->GetCurrentIsoSprite()->GetSpriteWithScaleForActorLookingInDirection( m_facingTowardsDirection );
	Vector3				isoSpriteScale		= spriteToRender->scale.GetAsVector3();

	spriteTransform.SetPosition	( drawPosition );
	spriteTransform.SetRotation	( Vector3( 0.f, cameraOrienation, 0.f) );
	spriteTransform.SetScale	( isoSpriteScale );

	g_theRenderer->UseShaderProgram( m_ignoreAlphaShaderProgram );
	spriteToRender->srcSprite->Render( spriteTransform.GetTransformMatrix() );
	g_theRenderer->UseShaderProgram( nullptr );
}

Ability* Actor::IsTakingTurn_Update( float deltaSeconds, bool& out_finishedTakingTurn )
{
	Ability*		out_finalizedActionForThisTurn	= nullptr;
	static Ability*	createdAction					= nullptr;
					out_finishedTakingTurn			= false;

	switch ( m_currentTurnState )
	{
	case ACTION_SELECTION:
		g_currentBattle->MoveIndicatorToCurrentPlayerPosition();
		g_currentBattle->FocusCameraOnCurrentPlayer	 ( deltaSeconds );
		g_currentBattle->RotateCameraWithRightStick	 ( deltaSeconds );
		g_currentBattle->ZoomTheCameraBasedOnTriggers( deltaSeconds );

		m_actionsMenu->Update(deltaSeconds);
		return nullptr;

		break;

	case ACTION_FINALIZING:
		// Create that Ability Action
		if( createdAction == nullptr )
			createdAction = Ability::ConstructAbilityNamedForActor( m_selectedActionName, *this );

		// Let player finish his action instruction, so we can that action has all the data it needs to execute afterwards
		bool actionCancelled = false;
		bool actionFinalized = createdAction->LetPlayerFinalizeTheAction_Update( actionCancelled, deltaSeconds );

		// Wait till action gets finalized
		if( !actionFinalized )
			return nullptr;

		// Action is finalized, now
		if( actionCancelled )
		{
			delete createdAction;
			createdAction					= nullptr;
			out_finalizedActionForThisTurn	= nullptr;
			m_currentTurnState				= ACTION_SELECTION;
		}
		else
		{
			// Out thisActions, to handover
			numActionsPerformedThisTurn++;
			out_finalizedActionForThisTurn = createdAction;
			createdAction = nullptr;
		}

		ResetActionSelected();
		break;
	};

	// If finished taking turn
	if( numActionsPerformedThisTurn >= m_totalActionsAllowed )
	{
		out_finishedTakingTurn			= true;
		numActionsPerformedThisTurn		= 0;
	}
	
	return out_finalizedActionForThisTurn;
}

void Actor::IsTakingTurn_Render() const
{
	// Render UI Menu, if Actor isTakingTurn
	m_actionsMenu->Render();
}

Vector3 Actor::GetPosition() const
{
	return m_position;
}

void Actor::SetPosition( Vector3 const & newPosition )
{
	m_position = newPosition;
}

void Actor::ActionSelected( const char* actionName )
{
	m_currentTurnState			= ACTION_FINALIZING;
	m_selectedActionName		= actionName;
}

void Actor::ResetActionSelected()
{
	m_currentTurnState			= ACTION_SELECTION;
	m_selectedActionName		= "";
}

void Actor::IncreaseWaitingTimeBy( float timeIncrease /* = 1 */ )
{
	m_waitingTime += timeIncrease;
}

void Actor::JustAttacked()
{
	m_waitingTime += m_costToAttack;
}

void Actor::JustMoved()
{
	m_waitingTime += m_costToMove;
}

void Actor::StartedWaiting( int canWaitUpto /* = -1 */ )
{
	if( canWaitUpto > m_costToWait )		// Wait for maximum amount possible
		m_waitingTime += canWaitUpto;
	else									// Else, wait for minimum amount possible
		m_waitingTime += m_costToWait;
}

bool Actor::IsReadyForTurn() const
{
	return m_waitingTime <= 0;
}

std::vector< IntVector2 > Actor::CanMoveToLocationsAccordingToHeatMap() const
{
	std::vector< IntVector2> availableLocations;
	Vector3 currPosition = GetPosition();
	HeatMap heatMap = m_currentMap.GenerateHeatMapForPositionAndJumpHeight( (int)currPosition.x, (int)currPosition.z, m_definition->m_jumpHeight );
	
	for( int x = 0; x < heatMap.m_mapDimension.x ; x++ )
	{
		for( int z = 0; z < heatMap.m_mapDimension.y; z++ )
		{
			IntVector2 heatAtCoord = IntVector2(x,z);

			if( heatMap.GetHeat( heatAtCoord ) <= m_definition->m_moveRange )
				availableLocations.push_back( heatAtCoord );
		}
	}

	return availableLocations;
}