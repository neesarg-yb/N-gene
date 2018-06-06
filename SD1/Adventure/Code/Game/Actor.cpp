#pragma once
#include <algorithm>
#include "Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/theGame.hpp"
#include "Game/Projectile.hpp"

Actor::Actor( Vector2 position, float orientationInDegrees, std::string entityDefName )
	: Entity( position, orientationInDegrees, *ActorDefinition::s_definitions[ entityDefName ] )
{
	m_actorDefinition = (ActorDefinition*) m_definition;
	m_currentHealth = m_actorDefinition->m_startHealth;

	m_goalPosition = Vector2( (float)GetRandomFloatInRange( -100, 100 ) , (float)GetRandomFloatInRange( -100, 100 ) );

	// Clone all the AIBehaviors
	std::vector< AIBehavior* > behaviorsFromDef = static_cast< ActorDefinition* >(m_definition)->m_AIBehaviors;
	for( unsigned int i=0; i < behaviorsFromDef.size(); i++ )
	{
		AIBehavior* newBehavior = behaviorsFromDef[i]->CloneMeForActor( *this );
		m_AIBehaviors.push_back( newBehavior );
	}
}

Actor::~Actor()
{
	for( unsigned int i = 0; i < m_AIBehaviors.size(); i++ )
	{
		delete m_AIBehaviors[i];
		m_AIBehaviors[i] = nullptr;

		m_AIBehaviors.erase( m_AIBehaviors.begin() + i );
		i--;
	}
}

void Actor::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);

	PerformBehaviourActions( deltaSeconds );

	// AIUtility based currentAIBehavior
	std::vector< float > utilitiesPerBehavior;
	for( unsigned int i = 0; i < m_AIBehaviors.size(); i++ )
	{
		Map&	currentMap		= *g_theGame->m_currentAdventure->m_currentMap;
		Player& currentPlayer	=  currentMap.m_currentPlayer;
		float	utility			=  m_AIBehaviors[i]->CalculateUtility( currentMap, currentPlayer );

		utilitiesPerBehavior.push_back( utility );
	}

	// Find and set that AIBehavior as currentAIBehavior
	std::vector< float >::iterator maxUtilIterator = max_element( utilitiesPerBehavior.begin(), utilitiesPerBehavior.end() );
	if( maxUtilIterator != utilitiesPerBehavior.end() )
	{
		int behaviorIndex	= maxUtilIterator - utilitiesPerBehavior.begin();
		m_currentAIBehavior = m_AIBehaviors[ behaviorIndex ];
	}

	// call Update() on the current AIBehavior
	if( m_currentAIBehavior != nullptr ) 
	{
		m_currentAIBehavior->Update( deltaSeconds );
	}

}

void Actor::Render()
{
	Entity::Render();
}

void Actor::PerformBehaviourActions( float deltaSeconds )
{
	m_timeElaspedSinceLastShooting += deltaSeconds;
}

Projectile* Actor::CreateAProjectile( float shootingRate )
{
	if( m_timeElaspedSinceLastShooting > (1.f / shootingRate) ) {
		// Create new projectile
		Vector2 playerPosition = g_theGame->m_currentAdventure->m_currentMap->m_currentPlayer.m_position;
		Vector2 dirToShoot			= playerPosition - m_position;
		float	degreeToShootAt		= dirToShoot.GetOrientationDegrees();
		Projectile* newProjectile	= new Projectile( m_position, degreeToShootAt, "Fire", static_cast<ActorDefinition*>(m_definition)->m_faction );

		// Reset the timeElasped
		m_timeElaspedSinceLastShooting = 0.f;

		return newProjectile;
	}
	else {
		// Don't create a new one
		return nullptr;
	}
}

void Actor::UpdateTheHeatMap( HeatMap* theHeatMap, const Map& currentAdventureMap )
{
	// TODO: Optimize the this function

	// If no heatMap initialized, return
	if( theHeatMap == nullptr )
		return;


	// If map got changed, change the dimension and defaultValue of theHeatMap, as well
	IntVector2 heatMapDimension			= theHeatMap->m_mapDimension;
	IntVector2 adventureMapDimension	= currentAdventureMap.m_dimension;
	if( heatMapDimension != adventureMapDimension )
	{
		float newDefaultValue			= ( adventureMapDimension.x * adventureMapDimension.y ) + 99.f;
		
		delete theHeatMap;
		theHeatMap = nullptr;

		theHeatMap			= new HeatMap( adventureMapDimension, newDefaultValue );
		heatMapDimension	= adventureMapDimension;

		// Set actor_position's heat to ZERO
		IntVector2 playerOnTile = IntVector2( m_position );
		theHeatMap->SetHeat( 0.f, playerOnTile );
	}


	// HeatMap Generation
	bool changedAHeatValue;

	do 
	{
		changedAHeatValue = false;

		// for all cells in HeatMap
		for( int y = 0; y < heatMapDimension.y; y++ )
		{
			for( int x = 0; x < heatMapDimension.x; x++ )
			{
				IntVector2 cellCoords = IntVector2( x, y );
				
				// Not having default Value
				float thisCellHeatValue = theHeatMap->GetHeat( cellCoords );
				if( thisCellHeatValue == theHeatMap->m_initialHeatValue )
					continue;

				// Get surrounding cells
				IntVector2 surroundingCells[4]	=	{ cellCoords + Vector2::TOP_DIR, cellCoords + Vector2::BOTTOM_DIR, cellCoords + Vector2::RIGHT_DIR, cellCoords + Vector2::LEFT_DIR };

				for( int sCell = 0; sCell < 4; sCell++ )
				{
					IntVector2& sCellCoords = surroundingCells[ sCell ];

					// If it isn't a valid cell
					if( sCellCoords.x >= heatMapDimension.x || sCellCoords.y >= heatMapDimension.y ||
						sCellCoords.x <  0					|| sCellCoords.y <  0 )
						continue;

					// Check for heatValue change
					const float costToMoveAStep = currentAdventureMap.m_tiles[ GetIndexFromColumnRowNumberForMatrixOfWidth( sCellCoords.x, sCellCoords.y, currentAdventureMap.m_dimension.x) ].m_tileDef->m_allowsWalking ? 1.f : theHeatMap->m_initialHeatValue;
					const float totalCost = thisCellHeatValue + costToMoveAStep;
					if( totalCost < theHeatMap->GetHeat( sCellCoords ) )
					{
						theHeatMap->SetHeat( totalCost, sCellCoords );
						changedAHeatValue = true;
					}
				}
			}
		}
	} while ( changedAHeatValue );
}