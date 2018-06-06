#include "PortalDefinition.hpp"

std::map< std::string, PortalDefinition* > PortalDefinition::s_definitions;

PortalDefinition::PortalDefinition( const XMLElement& definitionElement )
	:EntityDefinition( definitionElement )
{
	// TODO: SET APPROPRIATE VALUES OF VARIABLES FROM ENTITY_DEFINITION, WHICH ARE NOT DESCRIBED IN XML FILE
}

PortalDefinition::~PortalDefinition()
{

}

void PortalDefinition::LoadDefinitions( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "PortalDefinitions", "Root element of Portals.xml should to be PortalDefinitions..!" );

	for( XMLElement* thisChild = (XMLElement*) root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is ProjectileDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisChild->Name() ) == "PortalDefinition", "XML Element named wrong( " + std::string( thisChild->Name() ) + " ). It should be named PortalDefinition..!" );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, PortalDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			PortalDefinition* newDefToPush = new PortalDefinition( *thisChild );
			s_definitions[ nameOfCurrDef ] = newDefToPush;
		}
	}
}