#pragma once
#include <map>
#include <string>
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/ParserMAP/BrushMAP.hpp"

typedef std::map< std::string, std::string > NameValueMap;

class EntiryMAP
{
public:
	 EntiryMAP();
	~EntiryMAP();

public:
	// Properties
	std::string		m_className = "";
	NameValueMap	m_properties;

private:
	// Geometry
	std::vector< BrushMAP > m_geometry;

public:
	int				GetGeometryCount() const;
	MeshBuilder*	ConstructMeshBuilderForGeometryAtIndex( int gIdx ) const;
	Renderable*		ConstructRenderableForGeometryAtIndex( int gIdx ) const;

public:
	static bool		LoadFromFile( char const * mapFilePath, EntiryMAP* &entityMapPtr_out );
};
