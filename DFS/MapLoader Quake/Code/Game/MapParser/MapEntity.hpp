#pragma once
#include <map>
#include <string>
#include "Engine/File/File.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/MapParser/MapBrush.hpp"
#include "Game/MapParser/MapFileBuffer.hpp"

typedef std::map< std::string, std::string > NameValueMap;

class MapParser;

class MapEntity
{
	friend MapParser;

public:
	 MapEntity();
	~MapEntity();

public:
	// Properties
	std::string		m_className = "";
	NameValueMap	m_properties;

private:
	// Geometry
	std::vector< MapBrush > m_geometry;

public:
	// Properties
	void			SetProperty( std::string const &pName, std::string const &pValue );		// Overwrites, if already exists

	// Geometry
	int				GetGeometryCount() const;
	MeshBuilder*	ConstructMeshBuilderForGeometryAtIndex( int gIdx ) const;
	Renderable*		ConstructRenderableForGeometryAtIndex( int gIdx ) const;

private:
	static MapEntity*	ParseFromBuffer	( MapFileBuffer &buffer );							// Returns nullptr on failure
	static bool			ParseProperty	( MapFileBuffer &buffer, std::string &pName_out, std::string &pValue_out );
};
