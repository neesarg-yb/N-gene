#pragma once
#include "Game/AIBehavior.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class AIBehavior_Follow: public AIBehavior
{
public:
	 AIBehavior_Follow( const XMLElement& followBehaviourElement );
	~AIBehavior_Follow();

public:
	float		m_range	= 5.f;
	Map*		m_currentMap = nullptr;
	Player*		m_currentPlayer = nullptr;

	void		Update( float deltaSeconds );

	AIBehavior* CloneMeForActor( Actor& hostActor ) const;
	float		CalculateUtility( Map& currentMap, Player& currentPlayer );

private:
	Vector2 m_goalPosition	= Vector2::ZERO;
};