#pragma once
#include "Game/AIBehavior.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class AIBehavior_Attack: public AIBehavior
{
public:
	 AIBehavior_Attack( const XMLElement& followBehaviourElement );
	~AIBehavior_Attack();

public:
	float		m_range			= 5.f;
	float		m_rateOfFire	= 1.f;
	Map*		m_currentMap	= nullptr;
	Player*		m_currentPlayer = nullptr;

	void		Update( float deltaSeconds );

	AIBehavior* CloneMeForActor( Actor& hostActor ) const;
	float		CalculateUtility( Map& currentMap, Player& currentPlayer );

private:
	Vector2 m_goalPosition	= Vector2::ZERO;
};