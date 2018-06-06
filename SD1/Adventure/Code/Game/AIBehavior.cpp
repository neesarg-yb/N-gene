#pragma once
#include "AIBehavior.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/AIBehavior_Wander.hpp"
#include "Game/AIBehavior_Follow.hpp"
#include "Game/AIBehavior_Attack.hpp"
#include "Game/AIBehavior_FollowPlayer.hpp"

AIBehavior::AIBehavior( const XMLElement& specificBehaviourElement )
{
	m_utility = ParseXmlAttribute( specificBehaviourElement, "initialUtility", m_utility );
}

AIBehavior*	AIBehavior::ConstructAIBehaviorForXMLElement( const XMLElement& specificBehviorElement )
{
	AIBehavior* toReturn = nullptr;

	// TODO: Implement Factory method, here
	std::string behaviorName = specificBehviorElement.Name();

	if( behaviorName == "Wander" )
		toReturn = new AIBehavior_Wander( specificBehviorElement );
	else if ( behaviorName == "Follow" )
		toReturn = new AIBehavior_Follow( specificBehviorElement );
	else if ( behaviorName == "Attack" )
		toReturn = new AIBehavior_Attack( specificBehviorElement );
	else if ( behaviorName == "FollowPlayer" )
		toReturn = new AIBehavior_FollowPlayer( specificBehviorElement );	

	GUARANTEE_RECOVERABLE( toReturn != nullptr, std::string("FactoryFunction: AIBehavior_" + behaviorName + ", not found!!") );

	return toReturn;
}