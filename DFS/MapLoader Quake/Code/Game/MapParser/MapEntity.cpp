#pragma once
#include "MapEntity.hpp"
#include "Engine/Math/ConvexPolyhedron.hpp"
#include "Game/GameCommon.hpp"

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

int MapEntity::GetBrushCount() const
{
	return (int)m_brushes.size();
}

Renderable* MapEntity::ConstructRenderableForBrushAtIndex( int bIdx ) const
{
	std::vector< Plane3 > brushPlanes;
	MapBrush const &operationBrush = m_brushes[ bIdx ];
	for( int p = 0; p < operationBrush.m_planes.size(); p++ )
	{
		MapPlane const &parsedPlane = operationBrush.m_planes[p];

		// Get the normal
		// Vector3 aToB = parsedPlane.planeDescriptionPoints[1] - parsedPlane.planeDescriptionPoints[0];
		// Vector3 bToC = parsedPlane.planeDescriptionPoints[2] - parsedPlane.planeDescriptionPoints[1];
		// Vector3 normal = Vector3::CrossProduct( aToB, bToC ).GetNormalized();

		// TODO( "How to calculate Normal for each brush?" );
 		// Vector3 cToB = parsedPlane.planeDescriptionPoints[2] - parsedPlane.planeDescriptionPoints[1];
 		// Vector3 bToA = parsedPlane.planeDescriptionPoints[1] - parsedPlane.planeDescriptionPoints[0];
 		// Vector3 normal = Vector3::CrossProduct( cToB, bToA ).GetNormalized();

		Vector3 const &p0 = parsedPlane.planeDescriptionPoints[0];
		Vector3 const &p1 = parsedPlane.planeDescriptionPoints[1];
		Vector3 const &p2 = parsedPlane.planeDescriptionPoints[2];
		Vector3 normal	= Vector3::CrossProduct( p2 - p1, p0 - p1 ).GetNormalized();
		float	dist	= Vector3::DotProduct( normal, p1 );

		// Create the plane
		Plane3 newPlane = Plane3( normal, dist );
		brushPlanes.push_back( newPlane );
	}

	ConvexPolyhedron brushPolygon;
	for( int p = 0; p < brushPlanes.size(); p++ )
		brushPolygon.AddPlane( brushPlanes[p] );

	brushPolygon.Rebuild( FLOAT_ERROR_TOLERANCE_MAPFILE );
	Mesh* mesh = brushPolygon.ConstructMesh( RGBA_WHITE_COLOR );
	Material* mat = Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	return new Renderable( mesh, mat );
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
				
				if( propName == "classname" )
					entity->m_className = propValue;
			}
		}
		else if( indicatorChar == '{' )											// A BRUSH
		{
			MapBrush* parsedBrush = nullptr;
			parsedBrush  = MapBrush::ParseFromBuffer( buffer );

			if( parsedBrush != nullptr )
			{
				// Success
				entity->m_brushes.push_back( *parsedBrush );

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
