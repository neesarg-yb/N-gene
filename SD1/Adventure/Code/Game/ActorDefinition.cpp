#include "ActorDefinition.hpp"

std::map< std::string, ActorDefinition* > ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition( const XMLElement& definitionElement )
	:EntityDefinition( definitionElement )
{
	m_faction		= ParseXmlAttribute( definitionElement, "faction", m_faction );

	XMLElement* healthElement = nullptr;
	healthElement = (XMLElement*) definitionElement.FirstChildElement( "Health" );
	
	GUARANTEE_OR_DIE( healthElement != nullptr, std::string("Can't find Health in ActodDefinition of " + m_name + "..!" ) );

	m_startHealth	= ParseXmlAttribute( *healthElement, "start", m_startHealth );
	m_maxHealth		= ParseXmlAttribute( *healthElement, "max", m_maxHealth );

	// AI Behaviors
	XMLElement* behaviorsElement = nullptr;
	behaviorsElement = (XMLElement*) definitionElement.FirstChildElement("Behaviors");
	if( behaviorsElement != nullptr )
	{
		// For all specific AIBehaviors
		for( XMLElement* specificAIBehavior = behaviorsElement->FirstChildElement(); specificAIBehavior != nullptr; specificAIBehavior = specificAIBehavior->NextSiblingElement() )
		{
			AIBehavior* newBehavior = AIBehavior::ConstructAIBehaviorForXMLElement( *specificAIBehavior );
			if( newBehavior != nullptr )
				m_AIBehaviors.push_back( newBehavior );
		}
	}
}

ActorDefinition::~ActorDefinition()
{

}

void ActorDefinition::LoadDefinitions( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "ActorDefinitions", "Root element of Actors.xml should to be ActorDefinitions..!" );

	for( XMLElement* thisChild = (XMLElement*) root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is TileDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisChild->Name() ) == "ActorDefinition", "XML Element named wrong( " + std::string( thisChild->Name() ) + " ). It should be named ActorDefinition..!" );

		std::string	nameOfCurrDef = ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, ActorDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			ActorDefinition* newDefToPush = new ActorDefinition( *thisChild );
			s_definitions[ nameOfCurrDef ] = newDefToPush;
		}
	}
}