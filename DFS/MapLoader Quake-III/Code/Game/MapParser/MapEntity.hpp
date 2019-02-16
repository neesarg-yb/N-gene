#pragma once
#include <map>
#include <string>
#include "Engine/File/File.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/MapParser/MapBrush.hpp"

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
	int				GetGeometryCount() const;
	MeshBuilder*	ConstructMeshBuilderForGeometryAtIndex( int gIdx ) const;
	Renderable*		ConstructRenderableForGeometryAtIndex( int gIdx ) const;

private:
	static bool		ParseFromLines( std::vector<std::string> &linesOfEntity, MapEntity* &entity_out );
};
