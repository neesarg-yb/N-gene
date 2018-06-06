#pragma once
#include "Game/AIBehavior.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class AIBehavior_FollowPlayer: public AIBehavior
{
public:
	 AIBehavior_FollowPlayer( const XMLElement& followBehaviourElement );
	~AIBehavior_FollowPlayer();

public:
	Map*		m_currentMap = nullptr;
	Player*		m_currentPlayer = nullptr;

	void		Update( float deltaSeconds );

	AIBehavior* CloneMeForActor( Actor& hostActor ) const;
	float		CalculateUtility( Map& currentMap, Player& currentPlayer );

private:
	Vector2 m_goalPosition	= Vector2::ZERO;
};