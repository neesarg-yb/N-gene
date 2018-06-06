#include "ProjectileDefinition.hpp"

std::map< std::string, ProjectileDefinition* > ProjectileDefinition::s_definitions;

ProjectileDefinition::ProjectileDefinition( const XMLElement& definitionElement )
	:EntityDefinition( definitionElement )
{
	XMLElement* damageElement = nullptr;
	damageElement = (XMLElement*) definitionElement.FirstChildElement( "Damage" );

	GUARANTEE_OR_DIE( damageElement != nullptr, std::string("Can't find Damage in ProjectileDefinition of " + m_name + "..!" ) );

	FloatRange damageRange = ::ParseXmlAttribute( *damageElement, "amount", damageRange );
	m_damageAmount	= GetRandomFloatInRange( damageRange.min, damageRange.max );

	// TODO: SET APPROPRIATE VALUES OF VARIABLES FROM ENTITY_DEFINITION, WHICH ARE NOT DESCRIBED IN XML FILE
}

ProjectileDefinition::~ProjectileDefinition()
{

}

void ProjectileDefinition::LoadDefinitions( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "ProjectileDefinitions", "Root element of Projectiles.xml should to be ProjectileDefinitions..!" );

	for( XMLElement* thisChild = (XMLElement*) root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is TileDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisChild->Name() ) == "ProjectileDefinition", "XML Element named wrong( " + std::string( thisChild->Name() ) + " ). It should be named ProjectileDefinition..!" );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, ProjectileDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			ProjectileDefinition* newDefToPush = new ProjectileDefinition( *thisChild );
			s_definitions[ nameOfCurrDef ] = newDefToPush;
		}
	}
}