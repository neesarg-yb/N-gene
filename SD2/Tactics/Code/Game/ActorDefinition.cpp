#pragma once
#include "ActorDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

std::map< std::string, ActorDefinition* >	ActorDefinition::s_actorDefinitions;

ActorDefinition::ActorDefinition( const XMLElement& definitionElement )
{
	std::string definitionName = ParseXmlAttribute( definitionElement, "name", std::string("ACT_DEF_NAME_NOTFOUND") );

	// Size Element
	const XMLElement* sizeElement  = definitionElement.FirstChildElement( "Size" );
	GUARANTEE_OR_DIE( sizeElement != nullptr, std::string("XML Parsing Error: Can't find Size in ActorDefinition of " + definitionName + "..!" ) );
	float height	= ParseXmlAttribute( *sizeElement, "height", 0.f );
	float width		= ParseXmlAttribute( *sizeElement, "width" , 0.f );
	GUARANTEE_OR_DIE( height != 0.f && width != 0.f, std::string("Height or width can't be zero for " + definitionName + "'s definition..!" ) );
	m_dimension		= Vector3( width, height, width );

	// Render Element
	const XMLElement* renderElement	= definitionElement.FirstChildElement( "Render" );
	GUARANTEE_OR_DIE( renderElement != nullptr, std::string("XML Parsing Error: Can't find Render in ActorDefinition of " + definitionName + "..!" ) );
	m_tintColor		= ParseXmlAttribute( *renderElement, "colorTint", RGBA_WHITE_COLOR );
	m_anchorBoxSize = ParseXmlAttribute( *renderElement, "anchorBoxSize", m_dimension );		// If anchorBoxSize not found, assume that it will be equal to m_dimension
	m_anchorPoint	= ParseXmlAttribute( *renderElement, "anchorPoint", m_anchorPoint );
	m_animationSet	= ParseXmlAttribute( *renderElement, "animationSet", std::string("") );

	// Health Element
	const XMLElement* healthElement = definitionElement.FirstChildElement( "Health" );
	GUARANTEE_OR_DIE( healthElement != nullptr, std::string("XML Parsing Error: Can't find Health in ActorDefinition of " + definitionName + "..!" ) );
	m_maxHealth		= ParseXmlAttribute( *healthElement, "max", m_maxHealth );

	// Movement Element
	TODO("Get rid of it & Switch to Move Action, eventually..!");
	const XMLElement* movementElement = definitionElement.FirstChildElement( "Movement" );
	GUARANTEE_OR_DIE( movementElement != nullptr, std::string("XML Parsing Error: Can't find Movement in ActorDefinition of " + definitionName + "..!" ) );
	m_moveRange		= ParseXmlAttribute( *movementElement, "range", m_moveRange );
	m_jumpHeight	= ParseXmlAttribute( *movementElement, "jump", m_jumpHeight );

	// Abilities Element
	const XMLElement* abilitiesElement = definitionElement.FirstChildElement( "Abilities" );
	GUARANTEE_OR_DIE( abilitiesElement != nullptr, std::string("XML Parsing Error: Can't find Abilities in ActorDefinition of " + definitionName + "..!" ) );
	for( const XMLElement* thisAbility = abilitiesElement->FirstChildElement(); thisAbility != nullptr; thisAbility = thisAbility->NextSiblingElement() )
	{
		std::string abilityName				= thisAbility->Name();
		AbilityData* newAbilityToAdd		= new AbilityData( *thisAbility );
		m_dataOfAbilities[abilityName]		= newAbilityToAdd;
	}
}

ActorDefinition::~ActorDefinition()
{
	for( std::map< std::string, AbilityData*>::iterator it = m_dataOfAbilities.begin(); it != m_dataOfAbilities.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	m_dataOfAbilities.clear();
}

void ActorDefinition::LoadActorDefinitions( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "ActorDefinitions", "Root of Actors.xml should be ActorDefinitions..!");

	for( const XMLElement* thisChild = root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		std::string thisChildName = thisChild->Name();
		GUARANTEE_RECOVERABLE( thisChildName == "ActorDefinition", "XML Element named wrong( " + thisChildName + " ). It should be named ActorDefinition..!" );

		ActorDefinition* defToAdd = new ActorDefinition( *thisChild );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, ActorDefinition* >::iterator it = s_actorDefinitions.find( nameOfCurrDef );

		// If definition exists
		if( it != s_actorDefinitions.end() )
		{
			// Delete old definition
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = defToAdd;
		}
		else
			s_actorDefinitions[ nameOfCurrDef ] = defToAdd;
	}
}

void ActorDefinition::DeleteAllDefinitions()
{
	for( std::map< std::string, ActorDefinition* >::iterator it = s_actorDefinitions.begin(); it != s_actorDefinitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_actorDefinitions.clear();
}

