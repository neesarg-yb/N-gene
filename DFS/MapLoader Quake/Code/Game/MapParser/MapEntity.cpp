#pragma once
#include "MapEntity.hpp"

MapEntity::MapEntity()
{

}

MapEntity::~MapEntity()
{

}

void MapEntity::SetProperty( std::string const &pName, std::string const &pValue )
{
	m_properties[ pName ] = pValue;
}

MapEntity* MapEntity::ParseFromBuffer( MapFileBuffer &buffer )
{
	MapEntity *entity = new MapEntity();
	bool parseSuccess = true;

	// buffer's next character should be '{'
	if( buffer.ReadNextCharacter() != '{' )
		parseSuccess = false;

	while( parseSuccess )
	{
		buffer.SkipLeadingWhiteSpaces();

		// What's being parsed? A PROPERTY or A BRUSH
		char indicatorChar = buffer.PeekNextCharacter();
		if( indicatorChar == '"' )												// A PROPERTY
		{
			std::string propName, propValue;
			parseSuccess = MapEntity::ParseProperty( buffer, propName, propValue );

			if( parseSuccess )
			{
				// Success
				entity->SetProperty( propName, propValue );
			}
		}
		else if( indicatorChar == '{' )											// A BRUSH
		{
			MapBrush* parsedBrush = nullptr;
			parsedBrush  = MapBrush::ParseFromBuffer( buffer );

			if( parsedBrush != nullptr )
			{
				// Success
				entity->m_geometry.push_back( *parsedBrush );

				delete parsedBrush;
				parsedBrush = nullptr;
			}
			else
			{
				// Failure
				parseSuccess = false;
			}
		}
		else if( indicatorChar == '}' )											// END of Entity
		{
			buffer.ReadNextCharacter();
			break;
		}
		else
		{
			// Failure															// Unexpected Character Indicator
			parseSuccess = false;
		}
	}

	// Delete if not successful
	if( parseSuccess == false )
	{
		delete entity;
		entity = nullptr;
	}

	return entity;
}

bool MapEntity::ParseProperty( MapFileBuffer &buffer, std::string &pName_out, std::string &pValue_out )
{
	// name
	bool nameReadSuccess = buffer.ReadNextString( pName_out, '"' );
	
	// value
	buffer.SkipLeadingWhiteSpaces();
	bool valueReadSuccess = buffer.ReadNextString( pValue_out, '"' );

	return (nameReadSuccess && valueReadSuccess);
}
