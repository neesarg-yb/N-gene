#pragma once
#include "MapParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapParser::MapParser( MapFileBuffer &buffer, bool &parseSuccessful )
{
	while ( buffer.SeekNext('{') )	// Will put you right before the '{' character
	{
		MapEntity* parsedEntity = MapEntity::ParseFromBuffer( buffer );
		if( parsedEntity != nullptr )
		{
			m_entities.push_back( *parsedEntity );

			delete parsedEntity;
			parsedEntity = nullptr;
		}
		else
		{
			// Can't parse any more entities
			break;
		}
	}

	parseSuccessful = (m_entities.empty() == false);
}

MapParser::~MapParser()
{

}

MapParser* MapParser::LoadFromFile( const char *mapFilePath )
{
	MapFileBuffer buffer;
	MapParser* parsedMap = nullptr;

	if( buffer.LoadFromFile( mapFilePath ) )
	{
		bool parseSuccess = false;
		parsedMap = new MapParser( buffer, parseSuccess );

		// If it was a failure, cleanup
		if( parseSuccess == false )
		{
			delete parsedMap;
			parsedMap = nullptr;
		}
	}

	return parsedMap;
}
