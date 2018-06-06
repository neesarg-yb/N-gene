#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/AIBehavior.hpp"

class AIBehavior_Wander : public AIBehavior
{
public:
	 AIBehavior_Wander( const XMLElement& wanderElement );
	~AIBehavior_Wander();

public:
	int		m_proximity	= 5;
	Map*	m_currentMap = nullptr;

	void		Update( float deltaSeconds );

	AIBehavior* CloneMeForActor( Actor& hostActor ) const;
	float		CalculateUtility( Map& currentMap, Player& currentPlayer );

private:
	float	m_timeElaspedSinceLastGoalChanged	= 0.f;				// ----v
	float	m_goalPositionChangeAfterSeconds	= 5.f;				//	   TODO: Move both of these to class Actor
	Vector2 m_goalPosition						= Vector2::ZERO;
};