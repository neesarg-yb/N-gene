#pragma once
#include "Engine/File/File.hpp"
#include "Game/MapParser/MapEntity.hpp"

class MapParser
{
private:
	 MapParser( File &mapFile, bool &parseSuccessful );
	~MapParser();

public:
	std::vector< MapEntity > m_entities;

public:
	static bool LoadFromFile( const char *mapFilePath, MapParser* &parsedMap_out );
};
