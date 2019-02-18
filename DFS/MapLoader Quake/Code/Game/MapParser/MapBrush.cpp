#pragma once
#include "MapBrush.hpp"

MapPlane::MapPlane()
{

}

MapPlane::MapPlane( Vector3 *planeDescriptionThreePoints, std::string const &texName, Vector2 const &texPositionOffset, float texRotation, Vector2 const &texScale )
{
	this->planeDescriptionPoints[0] = planeDescriptionThreePoints[0];
	this->planeDescriptionPoints[1] = planeDescriptionThreePoints[1];
	this->planeDescriptionPoints[2] = planeDescriptionThreePoints[2];

	this->textureName = texName;

	this->texturePositionOffset	= texPositionOffset;
	this->textureRotation		= texRotation;
	this->textureScale			= texScale;
}

void MapBrush::AddPlane( MapPlane const &plane )
{
	m_planes.push_back( plane );
}

void MapBrush::AddPlane( Vector3 *planeDescriptionThreePoints, std::string const &texName, Vector2 const &texPositionOffset, float texRotation, Vector2 const &texScale )
{
	MapPlane planeToAdd( planeDescriptionThreePoints, texName, texPositionOffset, texRotation, texScale );
	AddPlane( planeToAdd );
}

MapBrush* MapBrush::ParseFromBuffer( MapFileBuffer &buffer )
{
	MapBrush* brush		= new MapBrush();
	bool parseSuccess	= true;

	// Brush has to start with '{'
	if( buffer.ReadNextCharacter() != '{' )
		parseSuccess = false;

	// Read each planes
	while( parseSuccess )
	{
		buffer.SkipLeadingWhiteSpaces();

		char indicatorChar = buffer.PeekNextCharacter();
		if( indicatorChar == '(' )												// A PLANE
		{
			MapPlane parsedPlane;

			// Plane Description Points
			bool p1ReadSuccess = buffer.ReadNextVector3( parsedPlane.planeDescriptionPoints[0] );
			buffer.SkipLeadingWhiteSpaces();
			bool p2ReadSuccess = buffer.ReadNextVector3( parsedPlane.planeDescriptionPoints[1] );
			buffer.SkipLeadingWhiteSpaces();
			bool p3ReadSuccess = buffer.ReadNextVector3( parsedPlane.planeDescriptionPoints[2] );

			// Texture Name
			buffer.SkipLeadingWhiteSpaces();
			bool texNameReadSuccess = buffer.ReadNextString( parsedPlane.textureName );

			// Texture Offsets
			buffer.SkipLeadingWhiteSpaces();
			bool texOffXReadSuccess = buffer.ReadNextFloat( parsedPlane.texturePositionOffset.x );
			buffer.SkipLeadingWhiteSpaces();
			bool texOffYReadSuccess = buffer.ReadNextFloat( parsedPlane.texturePositionOffset.y );

			// Texture Rotation
			buffer.SkipLeadingWhiteSpaces();
			bool texRotReadSuccess = buffer.ReadNextFloat( parsedPlane.textureRotation );

			// Texture Scale
			buffer.SkipLeadingWhiteSpaces();
			bool texScaleXSuccess = buffer.ReadNextFloat( parsedPlane.textureScale.x );
			buffer.SkipLeadingWhiteSpaces();
			bool texScaleYSuccess = buffer.ReadNextFloat( parsedPlane.textureScale.y );

			// Did it all succeed?
			bool planeParseSuccess = (p1ReadSuccess && p2ReadSuccess && p3ReadSuccess)
								  && (texNameReadSuccess) 
								  && (texOffXReadSuccess && texOffYReadSuccess)
								  && (texRotReadSuccess)
								  && (texScaleXSuccess && texScaleYSuccess);
			
			// Add plane to the brush
			if( planeParseSuccess == true )
				brush->AddPlane( parsedPlane );

			// Condition for while loop
			parseSuccess = planeParseSuccess;
		}
		else if( indicatorChar == '}' )											// END of Brush
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
		delete brush;
		brush = nullptr;
	}

	return brush;
}
