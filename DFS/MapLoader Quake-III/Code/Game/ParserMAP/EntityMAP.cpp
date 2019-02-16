#pragma once
#include "EntityMAP.hpp"

EntiryMAP::EntiryMAP()
{

}

EntiryMAP::~EntiryMAP()
{

}

int EntiryMAP::GetGeometryCount() const
{
	return -1;
}

MeshBuilder* EntiryMAP::ConstructMeshBuilderForGeometryAtIndex( int gIdx ) const
{
	UNUSED( gIdx );
	return nullptr;
}

Renderable* EntiryMAP::ConstructRenderableForGeometryAtIndex( int gIdx ) const
{
	UNUSED( gIdx );
	return nullptr;
}

bool EntiryMAP::LoadFromFile( char const * mapFilePath, EntiryMAP* &entityMapPtr_out )
{
	UNUSED( mapFilePath );
	entityMapPtr_out = nullptr;

	return false;
}
