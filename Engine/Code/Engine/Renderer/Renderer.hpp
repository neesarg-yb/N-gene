#pragma once

#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtil.hpp"

typedef unsigned int GLenum;

enum PrimitiveType
{
	PRIMITIVE_POINTS,		// in OpenGL, for example, this becomes GL_POINTS
	PRIMITIVE_LINES,		// in OpenGL, for example, this becomes GL_LINES
	PRIMITIVE_TRIANGES,		// in OpenGL, for example, this becomes GL_TRIANGLES
	PRIMITIVE_QUADS,		// in OpenGL, for example, this becomes GL_QUADS
	NUM_PRIMITIVE_TYPES
};

enum TextDrawMode
{
	TEXT_DRAW_OVERRUN,
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP,
	NUM_TEXT_DRAW_MODES
};

struct LoadedTexturesData {
	std::string pathToImage;
	Texture* texture;

	LoadedTexturesData( std::string path, Texture* newTexture) {
		pathToImage = path;
		texture = newTexture;
	}
};

const Rgba		 RGBA_BLACK_COLOR (   0,   0,   0, 255 );
const Rgba		 RGBA_WHITE_COLOR ( 255, 255, 255, 255 );
const Rgba		 RGBA_RED_COLOR	  ( 255,   0,   0, 255 );
const Rgba		 RGBA_GREEN_COLOR (   0, 255,   0, 255 );
const Rgba		 RGBA_BLUE_COLOR  (   0,   0, 255, 255 );

class Renderer
{
public:
	std::vector<LoadedTexturesData> texturePool;
	std::map< std::string , BitmapFont* > bitmapFontPool;

	Vector2 framesBottomLeft;
	Vector2 framesTopRight;
	Rgba	defaultInkColor;
	Rgba	defaultColor;
	float	defaultDrawingThickness;
	
	Renderer	();
	Renderer	( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness );
	~Renderer	();

	void BeginFrame	();
	void EndFrame	();

	void GLPushMatrix();
	void GLTranslate( float x, float y, float z);
	void GLRotate( float rotation, float x, float y, float z);
	void GLScale( float x, float y, float z);
	void GLPopMatrix();

	void GLBlendChangeBeforeAnimation();
	void GLBlendRestoreAfterAnimation();

	void SetOrtho			( const Vector2& bottomLeft, const Vector2& topRight );
	void ClearScreen		( const Rgba& clearColor );

	void DrawMeshImmediate	( const Vertex_3DPCU* vertexes, int numVertexes, PrimitiveType primitiveType );
	void DrawLine			( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness );
	void DrawFromVertexArray( const Vector2 vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale );
	void DrawPolygon		( const Vector2& center, float radius, float sides, float orientationAngle );
	void DrawDottedPolygon	( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color );

	void DrawAABB			( const AABB2& bounds, const Rgba& color );
	void DrawTexturedAABB	( const AABB2& bounds, const Texture& texture, 
							  const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, 
							  const Rgba& tint );
	void DrawTexturedAABBArray ( const Vertex_3DPCU* vertexes, int numVertexes, const Texture& texture );

	void DrawText2D		    ( const Vector2& drawMins, const std::string& asciiText,
							  float cellHeight, const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextInBox2D    ( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
							  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr, 
							  TextDrawMode drawMode = TEXT_DRAW_OVERRUN );

	Texture*	CreateOrGetTexture( const std::string& pathToImage );
 	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontName );
	
private:
	bool	findTextureFromPool( const std::string& pathToImage , Texture* &foundTexture );
	bool	findBitmapFontFromPool( const std::string& nameOfFont , BitmapFont* &foundFont );
	GLenum	GetAsOpenGLPrimitiveType( PrimitiveType inPrimitive ) const;

	void DrawTextAsOverrun		( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextAsShrinkToFit	( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextAsWordWrap		( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
};