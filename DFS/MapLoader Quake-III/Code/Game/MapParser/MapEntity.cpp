#pragma once
#include "MapEntity.hpp"

MapEntity::MapEntity()
{

}

MapEntity::~MapEntity()
{

}

int MapEntity::GetGeometryCount() const
{
	return -1;
}

MeshBuilder* MapEntity::ConstructMeshBuilderForGeometryAtIndex( int gIdx ) const
{
	UNUSED( gIdx );
	return nullptr;
}

Renderable* MapEntity::ConstructRenderableForGeometryAtIndex( int gIdx ) const
{
	UNUSED( gIdx );
	return nullptr;
}

bool MapEntity::ParseFromLines( std::vector<std::string> &linesOfEntity, MapEntity* &entity_out )
{
	MapEntity *thisEntity = new MapEntity();

	// Removing the starting and ending brackets { ... }
	linesOfEntity.pop_back();
	std::swap( linesOfEntity.front(), linesOfEntity.back() );
	linesOfEntity.pop_back();

	for( int lineNum = 0; lineNum < linesOfEntity.size(); lineNum++ )
	{
		int braketStack = 1;
		std::string &thisLine = linesOfEntity[ lineNum ];

		if( thisLine[0] == '"' )
		{
			std::string propertyName, propertyValue;

			bool nameFetched = FetchFirstString( thisLine, propertyName );
			RemoveInitialWhiteSpaces( thisLine );
			bool valueFetched = FetchFirstString( thisLine, propertyValue );

			if( (nameFetched && valueFetched) != true )
			{
				delete thisEntity;
				thisEntity = nullptr;

				return false;
			}

			if( propertyName == "classname" )
				thisEntity->m_className = propertyValue;
			else
				thisEntity->m_properties[ propertyName ] = propertyValue;
		}
		else if( thisLine[0] == '{' )
		{
			lineNum++;
			thisLine = linesOfEntity[ lineNum ];
			while( thisLine[0] != '}' )
			{
				Vector3		point1, point2, point3;
				std::string	textureName;
				float		texPosX, texPosY, texRot, texScaleX, texScaleY;

				bool fetchSuccess = true;
				fetchSuccess &= FetchVector3( thisLine, point1 );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchVector3( thisLine, point2 );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchVector3( thisLine, point3 );
				RemoveInitialWhiteSpaces( thisLine );

				fetchSuccess &= FetchNextWord( textureName );
				RemoveInitialWhiteSpaces( thisLine );

				fetchSuccess &= FetchNextFloat( texPosX );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchNextFloat( texPosY );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchNextFloat( texRot );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchNextFloat( texScaleX );
				RemoveInitialWhiteSpaces( thisLine );
				fetchSuccess &= FetchNextFloat( texScaleY );
				RemoveInitialWhiteSpaces( thisLine );

				if( !fetchSuccess )
				{
					delete thisEntity;
					thisEntity = nullptr;

					return false;
				}
				
				lineNum++;
			}
		}
	}
}
