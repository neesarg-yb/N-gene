#pragma once

#define WIN32_LEAN_AND_MEAN			// Always #define this before #including <windows.h>
#include <windows.h>				// #include this (massive, platform-specific) header in very few places
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Renderer/Renderer.hpp"


int g_openGlPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
	GL_QUADS			// called PRIMITIVE_QUADS		in our engine
};


Renderer::Renderer()
{
	framesBottomLeft = Vector2(0.f, 0.f);
	framesTopRight = Vector2(1000.f, 1000.f);
	defaultInkColor = Rgba(255, 255, 255, 255);
	defaultColor = Rgba(255, 255, 255, 255);
	defaultDrawingThickness = 1.5f;

	glLineWidth( defaultDrawingThickness );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

Renderer::Renderer( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness ) {
	framesBottomLeft = bottomLeft;
	framesTopRight = topRight;
	this->defaultInkColor = inkColor;
	this->defaultDrawingThickness = drawingThickness;
}

Renderer::~Renderer()
{
	
}

void Renderer::BeginFrame() {

}

void Renderer::EndFrame() {

}

void Renderer::ClearScreen( const Rgba& clearColor ) {
	float r, g, b, a;
	clearColor.GetAsFloats(r, g, b, a);					// [0, 255] to [0.f, 1.f]

	// Clear all screen (backbuffer) pixels to given color
	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::SetOrtho( const Vector2& bottomLeft, const Vector2& topRight ) {
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (100,100) top-right
	glLoadIdentity();
	glOrtho( bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f );
}

void Renderer::DrawMeshImmediate( const Vertex_3DPCU* vertexes, int numVertexes, PrimitiveType primitiveType )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer		( 3, GL_FLOAT,			sizeof( Vertex_3DPCU ), &vertexes[ 0 ].m_position );
	glColorPointer		( 4, GL_UNSIGNED_BYTE,	sizeof( Vertex_3DPCU ), &vertexes[ 0 ].m_color );
	glTexCoordPointer	( 2, GL_FLOAT,			sizeof( Vertex_3DPCU ), &vertexes[ 0 ].m_UVs );

	GLenum glPrimitiveType = g_openGlPrimitiveTypes[ primitiveType ];
	glDrawArrays( glPrimitiveType, 0, numVertexes );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

void Renderer::DrawLine( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness ) {
	// Setting line's thickness
	glLineWidth(lineThickness);
	glDisable( GL_TEXTURE_2D );

	Vertex_3DPCU verts[] = { 
		Vertex_3DPCU( start.GetAsVector3(), startColor, Vector2::ZERO ),
		Vertex_3DPCU( end.GetAsVector3(),	endColor,	Vector2::ZERO )
	};

	DrawMeshImmediate( verts, 2, PRIMITIVE_LINES );

	// Reset ink color & thickness
	glColor4ub(defaultInkColor.r, defaultInkColor.g, defaultInkColor.b, defaultInkColor.a);
	glLineWidth(defaultDrawingThickness);
}

void Renderer::DrawFromVertexArray( const Vector2 vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale ) {
	glPushMatrix();
	glTranslatef(center.x, center.y, 0.f);
	glRotatef(orientationDegree, 0.f, 0.f, 1.f);
	glScalef(scale, scale, scale);

	// For loop for every line
	for(int i=0; i<arraySize-1; i++) {
		DrawLine(vertex[i], vertex[i+1], defaultInkColor, defaultInkColor, defaultDrawingThickness);
	} 
	DrawLine(vertex[arraySize-1], vertex[0], defaultInkColor, defaultInkColor, defaultDrawingThickness);

	glPopMatrix();
}

void Renderer::DrawPolygon( const Vector2& center, float radius, float sides, float orientationAngle ) {
	// For loop for every line
	for(float i=0; i<sides; i++) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, defaultInkColor, defaultInkColor, defaultDrawingThickness);
	}
}

void Renderer::DrawDottedPolygon( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color ) {
	// For loop for every line
	for(float i=0; i<sides; i+=2) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, color, color, defaultDrawingThickness);
	}
}

void Renderer::DrawTexturedAABB( const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint ) {
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texture.m_textureID );

	Vertex_3DPCU verts[] = {
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y ) ),	// Upper-left
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMaxs.y ) ),	// Upper-right
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y ) ),	// Bottom-right
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMins.y ) )	// Bottom-left
	};

	DrawMeshImmediate( verts, 4, PRIMITIVE_QUADS );

	glColor4ub( defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a );

	glDisable( GL_TEXTURE_2D );
}

void Renderer::DrawTexturedAABBArray( const Vertex_3DPCU* vertexes, int numVertexes, const Texture& texture )
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texture.m_textureID );

	DrawMeshImmediate( vertexes, numVertexes, PRIMITIVE_QUADS );

	glColor4ub( defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a );

	glDisable( GL_TEXTURE_2D );
}

void Renderer::DrawText2D( const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	Vector2 newMins = drawMins;
	float cellWidth = cellHeight * font->GetGlyphAspect( asciiText.at(0) );
	Vector2 newMaxs = Vector2( drawMins.x + cellWidth , drawMins.y + cellHeight );
	AABB2 boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = font->GetUVsForGlyph( asciiText.at(i) );
		DrawTexturedAABB( boundForNextCharacter , font->m_spriteSheet.m_spriteSheetTexture , textCoords.mins , textCoords.maxs, tint );

		// Calculate bounds to draw next character
		newMins = Vector2( newMins.x + cellWidth , newMins.y);
		newMaxs = Vector2( newMaxs.x + cellWidth , newMaxs.y);
		boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	}
}

void Renderer::DrawTextInBox2D( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */, TextDrawMode drawMode /* = TEXT_DRAW_OVERRUN */ )
{
	switch (drawMode)
	{
		case TEXT_DRAW_OVERRUN:
			DrawTextAsOverrun( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		case TEXT_DRAW_SHRINK_TO_FIT:
			DrawTextAsShrinkToFit( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		case TEXT_DRAW_WORD_WRAP:
			DrawTextAsWordWrap( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		default:
			GUARANTEE_RECOVERABLE( false, std::string("No supported TextDrawMode found!") );
			break;
	}
}

void Renderer::DrawTextAsOverrun( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	std::vector< std::string > textLines;
	SetFromText( textLines, "\n", asciiText.c_str() );

	// For each line
	for( unsigned int i = 0; i < textLines.size(); i++ )
	{
		std::string thisLine = textLines[i];

		// Calculate Padding
		float boxWidth = drawInBox.maxs.x - drawInBox.mins.x;
		float widthOfLine = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
		float totalPaddingWidth = boxWidth - widthOfLine;

		float boxHeight = drawInBox.maxs.y - drawInBox.mins.y;
		float heightOfLine = desiredCellHeight;
		float totalPaddingHeight = boxHeight - heightOfLine;

		// Calculate drawMins considering the padding
		Vector2 drawMins = Vector2( drawInBox.mins.x + (totalPaddingWidth * alignment.x) , drawInBox.mins.y + (totalPaddingHeight * alignment.y) );
		// Modify drawMins's y-axis accoring to the line number
		drawMins.y -= heightOfLine * i;

		DrawText2D( drawMins, thisLine, desiredCellHeight, tint, font );
	}

}

void Renderer::DrawTextAsShrinkToFit( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	std::vector< std::string > textLines;
	SetFromText( textLines, "\n", asciiText.c_str() );
	
	
	std::string thisLine = textLines[0];
	float maxLineWidth = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
	// Get height of the whole text
	float totalTextHeight = textLines.size() * desiredCellHeight;
	
	// For each line
	for( unsigned int i = 1; i < textLines.size(); i++ )
	{
		thisLine = textLines[i];

		// Check for each line.. if they have larger width than maxLineWidth
		float widthOfLine = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
		maxLineWidth = widthOfLine > maxLineWidth ? widthOfLine : maxLineWidth;
	}

	float boxWidth		= drawInBox.maxs.x - drawInBox.mins.x;
	float boxHeight		= drawInBox.maxs.y - drawInBox.mins.y;
	float scaleWidth	= boxWidth / maxLineWidth;
	float scaleHeight	= boxHeight / totalTextHeight;

	// Set final scale based on width & height
	float finalScale = 1.f;
	if( scaleWidth < 1.f || scaleHeight < 1.f )
		finalScale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;
	
	// Calculate shifting for vertical alignment of the text-paragraph
	float heightToShift = (totalTextHeight-desiredCellHeight) * finalScale * (1 - alignment.y);		// (1 - alignment.y) because: if alignment.y == 0, draw as overrun will start drawing first line at bottom of the drawInBox
	AABB2 adjustedDrawBox = AABB2(  drawInBox.mins.x, drawInBox.mins.y + heightToShift,
									drawInBox.maxs.x, drawInBox.maxs.y + heightToShift );

	DrawTextAsOverrun( asciiText, alignment, adjustedDrawBox, desiredCellHeight*finalScale, tint, font );
}

void Renderer::DrawTextAsWordWrap( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	// TODO: Make this function work for multiple textLines

	// Make vector of words to combine them
	std::vector< std::string > allWords;
	SetFromText( allWords, " ", asciiText.c_str() );	// This function will split by " ", so  " " itself won't be included in each word
	for( std::string& thisWord : allWords )				// Append " " at back on every word, for simplicity
		thisWord += " ";

	// Make newLines according to words
	std::vector< std::string > newLines;
	newLines.push_back("");								// I'm doing this for upcoming newLines.back() function call on empty vector.
	
	float boxWidth = drawInBox.maxs.x - drawInBox.mins.x;

	for( std::string thisWord : allWords )
	{
		std::string& lastString_newLines = newLines.back();
		unsigned int totalChars_lastString = lastString_newLines.length();
		float totalWidth_newLine = (float)(thisWord.length() + totalChars_lastString) * desiredCellHeight * 1.f /* aspect ratio */;

		// If newLines last string's length is less than box width, append thisWord
		if( totalWidth_newLine <= boxWidth )
		{
			lastString_newLines += thisWord;
		}
		// Else pushBack thisWord
		else
		{
			newLines.push_back( thisWord );
		}
	}

	// Combine all newLines into one string separated by '\n'
	std::string bakedString;
	for( std::string aLine : newLines )
	{
		bakedString += aLine;
		bakedString += "\n";
	}

	// Remove the last, unnecessary "\n"
	bakedString = bakedString.substr(0, bakedString.size()-2);

	// Pass it to DrawTextAsShrinkToFit
	DrawTextAsShrinkToFit( bakedString, alignment, drawInBox, desiredCellHeight, tint, font );
}

void Renderer::DrawAABB( const AABB2& bounds, const Rgba& color ) {
	glDisable( GL_TEXTURE_2D );

	Vertex_3DPCU verts[] = {
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), color, Vector2::ZERO ),	// Upper-left
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), color, Vector2::ZERO ),	// Upper-right
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), color, Vector2::ZERO ),	// Bottom-right
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), color, Vector2::ZERO )	// Bottom-left
	};

	DrawMeshImmediate( verts, 4, PRIMITIVE_QUADS );

	glColor4ub( defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a );
}

Texture* Renderer::CreateOrGetTexture( const std::string& pathToImage ) {
	Texture* referenceToTexture = nullptr;															// to get texture from texturePool
	bool textureExistInPool = findTextureFromPool( pathToImage, referenceToTexture );				// find existing texture from texturePool

																									// if not found,
	if ( textureExistInPool == false ) {
		// create new texture and add it to the pool
		referenceToTexture = new Texture( pathToImage );

		LoadedTexturesData newTexData = LoadedTexturesData( pathToImage, referenceToTexture );
		texturePool.push_back(newTexData);
	}

	return referenceToTexture;
}
 
 BitmapFont* Renderer::CreateOrGetBitmapFont( const char* bitmapFontName )
 {
	 BitmapFont* fontToReturn = nullptr;

	 // If font already in the pool, return its pointer
	 bool fontExistsInPool = findBitmapFontFromPool( std::string(bitmapFontName) , fontToReturn );
	 if( fontExistsInPool )
	 {
		 return fontToReturn;
	 }
	 else
	 {
		 // Create path according to the string..
		 std::string pathToPNG = std::string("Data//Fonts//");
		 pathToPNG += bitmapFontName;
		 pathToPNG += ".png";

		 // Get Texture of fonts' sheet
		 Texture* bitmapFontTexture = CreateOrGetTexture(pathToPNG);

		 // Make a spritesheet out of it
		 SpriteSheet* bitmapFontSpritesheet = new SpriteSheet( *bitmapFontTexture , 16 , 16 );

		 // Make a bitmapFont out of it
		 fontToReturn = new BitmapFont( std::string(bitmapFontName) , *bitmapFontSpritesheet , 1.f );

		 // Add it to pool
		 bitmapFontPool[ std::string(bitmapFontName) ] = fontToReturn;

		 // return the bitmapFont
		 return fontToReturn;
	 }
	 
 }

bool Renderer::findTextureFromPool( const std::string& pathToImage , Texture* &foundTexture ) {
	
	for(unsigned int i=0; i<texturePool.size(); i++) {

		if( pathToImage == texturePool[i].pathToImage ) {
			foundTexture = texturePool[i].texture;
			return true;
		}
	}

	return false;
}

bool Renderer::findBitmapFontFromPool( const std::string& nameOfFont , BitmapFont* &foundFont )
{
	std::map< std::string , BitmapFont* >::iterator it = bitmapFontPool.find( nameOfFont );

	if( it != bitmapFontPool.end() )
	{
		foundFont = it->second;
		return true;
	}

	return false;
}

GLenum Renderer::GetAsOpenGLPrimitiveType( PrimitiveType inPrimitive ) const
{
	return g_openGlPrimitiveTypes[ inPrimitive ];
}


void Renderer::GLPushMatrix() {
	glPushMatrix();
}

void Renderer::GLTranslate( float x, float y, float z) {
	glTranslatef(x, y, z);
}

void Renderer::GLRotate( float rotation, float x, float y, float z) {
	glRotatef(rotation, x, y, z);
}

void Renderer::GLScale( float x, float y, float z) {
	glScalef(x, y, z);
}

void Renderer::GLPopMatrix() {
	glPopMatrix();
}

void Renderer::GLBlendChangeBeforeAnimation() {
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

void Renderer::GLBlendRestoreAfterAnimation() {
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}