#pragma once
#include "Engine/File/File.hpp"
#include "Game/MapParser/MapEntity.hpp"
#include "Game/MapParser/MapFileBuffer.hpp"

class MapParser
{
private:
	 MapParser( MapFileBuffer &buffer, bool &parseSuccessful );
	~MapParser();

public:
	std::vector< MapEntity > m_entities;

public:
	static MapParser* LoadFromFile( const char *mapFilePath );		// Returns nullptr on failure
};
