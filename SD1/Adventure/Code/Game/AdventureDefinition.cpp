#include "AdventureDefinition.hpp"

PortalData::PortalData( std::string portalType, std::string onTileType, std::string destinationMap, std::string reciprocalPortalType )
{
	this->portalType			= portalType;
	this->onTileType			= onTileType;
	this->destinationMap		= destinationMap;
	this->reciprocalPortalType	= reciprocalPortalType;
}

ActorData::ActorData( std::string actorType, int spawnCount, std::string onTileType )
{
	this->actorType				= actorType;
	this->spawnCount			= spawnCount;
	this->onTileType			= onTileType;
}

MapData::MapData( const XMLElement& mapElement )
{
	// Setup mapDefinition
	std::string mapName		= ParseXmlAttribute( mapElement, "name", std::string("") );
	std::string defName		= ParseXmlAttribute( mapElement, "mapDefinition", std::string("") );
	GUARANTEE_OR_DIE( defName != "", std::string("mapDefinition of map named " + mapName + " is not found in Adventures.xml!") );
	
	mapDefinition = MapDefinition::s_definitions[ defName ];

	// Setup all Portals
	for( XMLElement* portalElement = (XMLElement*) mapElement.FirstChildElement("Portal"); portalElement != nullptr; portalElement = portalElement->NextSiblingElement("Portal") )
	{
		std::string portalType				= ParseXmlAttribute( *portalElement, "type",			std::string("No data set") );
		std::string onTileType				= ParseXmlAttribute( *portalElement, "onTileType",		std::string("No data set") );
		std::string destinationMap			= ParseXmlAttribute( *portalElement, "toMap",			std::string("No data set") );
		std::string reciprocalPortalType	= ParseXmlAttribute( *portalElement, "reciprocalType",	std::string("No data set") );

		// Add new PortalData to vector
		PortalData *thisPortal = new PortalData( portalType, onTileType, destinationMap, reciprocalPortalType );
		portals.push_back( thisPortal );
	}

	// Setup all Actors
	for( XMLElement* actorElement = (XMLElement*) mapElement.FirstChildElement("Actor"); actorElement != nullptr; actorElement = actorElement->NextSiblingElement("Actor") )
	{
		int			spawnCount				= ParseXmlAttribute( *actorElement, "spawnCount",		1 );
		std::string actorType				= ParseXmlAttribute( *actorElement, "type",				std::string("No data set") );
		std::string onTileType				= ParseXmlAttribute( *actorElement, "onTileType",		std::string("No data set") );

		// Add new PortalData to vector
		ActorData *thisActor = new ActorData( actorType, spawnCount, onTileType );
		actors.push_back( thisActor );
	}
}

std::map< std::string, AdventureDefinition* > AdventureDefinition::s_definitions;

AdventureDefinition::AdventureDefinition( const XMLElement& definitionElement )
{
	// Set the attribute values
	m_adventureName			= ParseXmlAttribute( definitionElement, "name", m_adventureName );
	m_adventureTitle		= ParseXmlAttribute( definitionElement, "title", m_adventureName );

	// Set Start Conditions
	XMLElement* startCondElement = nullptr;
	startCondElement = (XMLElement*) definitionElement.FirstChildElement( "StartConditions" );

	GUARANTEE_OR_DIE( startCondElement != nullptr, std::string("Can't find StartConditions in AdventureDefinition of " + m_adventureName + "..!" ) );
	m_startMap				= ParseXmlAttribute( *startCondElement, "startMap", m_startMap );
	m_startOnTile			= ParseXmlAttribute( * startCondElement, "startOnTileType", m_startOnTile );

	// Set Victory Conditions, if any
	XMLElement* victoryCondElement = nullptr;
	victoryCondElement = (XMLElement*) definitionElement.FirstChildElement( "VictoryConditions" );

	if( victoryCondElement != nullptr ) {
		m_winOnDeathOf		= ParseXmlAttribute( *victoryCondElement, "haveDied", m_winOnDeathOf );
		m_winOnCollectionOf	= ParseXmlAttribute( *victoryCondElement, "collectionOf", m_winOnCollectionOf );
	}

	// For every Maps of this adventure, store all the data
	for( XMLElement* mapElement = (XMLElement*) definitionElement.FirstChildElement( "Map" ); mapElement != nullptr; mapElement = mapElement->NextSiblingElement( "Map" ) )
	{
		MapData *thisMapsData	= new MapData( *mapElement );
		std::string mapName		= ParseXmlAttribute( *mapElement, "name", std::string("Name not set") );

		// Add it to the registry
		m_maps[ mapName ] = thisMapsData;
	}
}

AdventureDefinition::~AdventureDefinition()
{

}

void AdventureDefinition::LoadDefinitions( const XMLElement& rootElement )
{
	std::string rootName = rootElement.Name();
	GUARANTEE_RECOVERABLE( rootName == "AdventureDefinitions", "Root element of Adventure.xml should to be AdventureDefinitions..!" );

	for( XMLElement* thisChild = (XMLElement*) rootElement.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is AdventureDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisChild->Name() ) == "AdventureDefinition", "XML Element named wrong( " + std::string( thisChild->Name() ) + " ). It should be named AdventureDefinition..!" );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, AdventureDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			AdventureDefinition* newDefToPush = new AdventureDefinition( *thisChild );
			s_definitions[ nameOfCurrDef ] = newDefToPush;
		}
	}
}