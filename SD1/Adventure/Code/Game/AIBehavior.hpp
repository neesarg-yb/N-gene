#pragma once
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"

class Map;
class Actor;
class Player;

class AIBehavior
{
public:
			 AIBehavior( const XMLElement& specificBehaviourElement );
	virtual ~AIBehavior() { };

public:
	float				m_utility	= 0.f;		// Range: [0, 1]
	Actor*				m_hostActor = nullptr;

	virtual float		CalculateUtility( Map& currentMap, Player& currentPlayer ) = 0;	

	virtual	void Update( float deltaSeconds ) = 0;															// It is critical to call CalculateUtility() before calling Update(), here!!!

	static	AIBehavior* ConstructAIBehaviorForXMLElement( const XMLElement& specificBehviorElement );		// Factory Function
	virtual AIBehavior* CloneMeForActor( Actor& hostActor ) const = 0;


private:

};