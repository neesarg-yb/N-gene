#pragma once
#include "MapParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

MapParser::MapParser( File &mapFile, bool &parseSuccessful )
{
	std::string thisLine;
	while( mapFile.ReadNextLine(thisLine) == true )
	{
		if( thisLine[0] == '{' )
		{
			std::vector< std::string > linesOfThisEntity;
			linesOfThisEntity.push_back( thisLine );

			int bracketStack = 1;
			while( bracketStack > 0 )
			{
				mapFile.ReadNextLine( thisLine );

				if( thisLine[0] == '}' )
					bracketStack--;
				else if( thisLine[0] == '{' )
					bracketStack++;

				linesOfThisEntity.push_back( thisLine );
			}

			MapEntity *thisEntity = nullptr;
			bool parseSuccess = MapEntity::ParseFromLines( linesOfThisEntity, thisEntity );

			if( parseSuccess )
			{
				m_entities.push_back( *thisEntity );

				delete thisEntity;
				thisEntity = nullptr;
			}
			else
			{
				parseSuccessful = false;
				return;
			}
		}
	}

	if( m_entities.size() == 0 )
		parseSuccessful = false;
	else
		parseSuccessful = true;
}

MapParser::~MapParser()
{

}

bool MapParser::LoadFromFile( const char *mapFilePath, MapParser* &parsedMap_out )
{
	File mapFile;
	bool fileOpened = mapFile.Open( mapFilePath, FILE_OPEN_MODE_READ );
	
	// If can't open the file
	if( !fileOpened )
	{
		parsedMap_out = nullptr;
		return false;
	}

	// Let's start parsing it!
	bool parsedSuccessfully = false;
	MapParser *parsedFile = new MapParser( mapFile, parsedSuccessfully );

	// If wasn't able to parse
	if( !parsedSuccessfully )
	{
		delete parsedFile;
		parsedFile = nullptr;

		parsedMap_out = nullptr;
		return false;
	}
	else
	{
		parsedMap_out = parsedFile;
		return true;
	}
}
