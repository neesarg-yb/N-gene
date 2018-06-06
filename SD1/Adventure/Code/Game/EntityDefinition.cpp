#include "EntityDefinition.hpp"

EntityDefinition::EntityDefinition( const XMLElement& definitionElement )
{
	// <Entity name="" faction="" tags="">
	m_name		= ParseXmlAttribute( definitionElement, "name", m_name );
	m_startTags	= ParseXmlAttribute( definitionElement, "tags", m_startTags );
	
	// <Size>
	XMLElement* sizeElement = nullptr;
	sizeElement = (XMLElement *) definitionElement.FirstChildElement("Size");
	GUARANTEE_RECOVERABLE( sizeElement != nullptr, std::string("<Size> for " + m_name + " entity, not found..!") );
	if( sizeElement != nullptr )
	{
		m_collisionRadius	= ParseXmlAttribute( *sizeElement, "physicsRadius", m_collisionRadius );
		m_visualLocalBounds	= ParseXmlAttribute( *sizeElement, "localDrawBounds", m_visualLocalBounds );
	}

	// <Movement>
	XMLElement* movementElement = nullptr;
	movementElement = (XMLElement *) definitionElement.FirstChildElement("Movement");
	GUARANTEE_OR_DIE( movementElement != nullptr, std::string("<Movement> for " + m_name + " actor, not found..!") );
	XMLElement* specificMovElement = nullptr;
	specificMovElement = (XMLElement *) movementElement->FirstChildElement();
	GUARANTEE_OR_DIE( specificMovElement != nullptr, std::string("Child of <Movement> for " + m_name + " actor, not found..!") );
	m_maxSpeed = ParseXmlAttribute( *specificMovElement, "speed", m_maxSpeed );			// TODO: In future I need to have variables for fly, swim & walk max-speed

	// <SpriteAnimSet>
	XMLElement* animSetElement = nullptr;
	animSetElement = (XMLElement *) definitionElement.FirstChildElement("SpriteAnimSet");
	GUARANTEE_OR_DIE( animSetElement != nullptr, std::string("<SpriteAnimSet> for " + m_name + " actor, not found..!") );
	m_animSetDef = new SpriteAnimSetDefinition( *animSetElement, *g_theRenderer );
}