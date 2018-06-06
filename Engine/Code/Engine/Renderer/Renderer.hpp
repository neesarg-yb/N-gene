#pragma once

#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Camera.hpp"

enum TextDrawMode
{
	TEXT_DRAW_OVERRUN,
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP,
	NUM_TEXT_DRAW_MODES
};

enum eCompare
{
	COMPARE_NEVER,			// GL_NEVER
	COMPARE_LESS,			// GL_LESS
	COMPARE_LEQUAL,			// GL_LEQUAL
	COMPARE_GREATER,		// GL_GREATER
	COMPARE_GEQUAL,			// GL_GEQUAL
	COMPARE_EQUAL,			// GL_EQUAL
	COMPARE_NOT_EQUAL,		// GL_NOTEQUAL
	COMPARE_ALWAYS,			// GL_ALWAYS
};

enum eRenderDataType
{
	RDT_FLOAT = 0,			// GL_FLOAT
	RDT_UNSIGNED_INT,		// GL_UNSIGNED_BYTE
	NUM_RDTs
};

struct LoadedTexturesData {
	std::string pathToImage;
	Texture* texture;

	LoadedTexturesData( std::string path, Texture* newTexture) {
		pathToImage = path;
		texture = newTexture;
	}
};

class Renderer
{
public:
	static Camera*		s_current_camera;

private:
	RenderBuffer*		m_temp_render_buffer	= nullptr;
	static Camera*		s_default_camera;
	static Sampler*		s_defaultSampler;
	static Texture*		s_defaultColorTarget;
	static Texture*		s_defaultDepthTarget;
	const  Texture*		m_defaultWhiteTexture	= nullptr;
	const  Texture*		m_currentTexture		= nullptr;
	const  Texture*		m_secondaryTexture		= nullptr;

	static Camera*		s_effectsCamera;
	static Texture*		s_sketchColorTarget;
	static Texture*		s_effectCurrentTarget;
	static Texture*		s_effectCurrentSource;

	Mesh*				m_immediateMesh			= nullptr;
	float				m_passFloatToShader = 0.f;

public:
	ShaderProgram* m_defaultShader = nullptr;
	ShaderProgram* m_currentShader = nullptr;
	static GLuint  s_default_vao;

	std::vector<LoadedTexturesData> texturePool;
	std::map< std::string , BitmapFont* > bitmapFontPool;
	std::map< std::string, ShaderProgram* > m_shaderProgramPool;

	Vector2 framesBottomLeft;
	Vector2 framesTopRight;
	Rgba	defaultInkColor;
	Rgba	defaultColor;
	float	defaultDrawingThickness;
	
	 Renderer	();
	 Renderer	( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness );
	~Renderer	();

	static bool		RendererStartup( void* hwnd );
	static void		RendererShutdown();
	static void		GLShutdown();

	ShaderProgram*	CreateOrGetShaderProgram( const char* name );
	void			LoadAllInbuiltShaders();																			// Also adds it to m_shaderProgramPool
	ShaderProgram*	LoadShaderProgramFromStrings( const char* name, const char* vs_program, const char* fs_program );	// Note! If same program exists, returns the existing one
	void			UseShaderProgram( ShaderProgram* useShader );
	void			SetPassFloatForShaderTo( float passFloatToShader );
	void			ResetPassFloatForShaderToZero();

	void ApplyEffect( ShaderProgram* effectShaderProgram );
	void EndEffect();

	void BeginFrame	();
	void EndFrame	();

	void GLPushMatrix();
	void GLTranslate( float x, float y, float z);
	void GLRotate( float rotation, float x, float y, float z);
	void GLScale( float x, float y, float z);
	void GLPopMatrix();

	static Vector3	GetDrawPositionUsingAnchorPoint( const Vector3& position, const Vector3& dimensions, const Vector3& anchorPoint = Vector3::ZERO );	// AnchorPoints: Bottom = (0, -1, 0), Left = (-1, 0, 0), Far = (1, 0, 0)

	void GLBlendChangeBeforeAnimation();
	void GLBlendRestoreAfterAnimation();

	void SetProjectionMatrix( float screen_height, float screen_near, float screen_far );								// takes (width, height, near, far)
	void ClearScreen		( const Rgba& clearColor );
	void ClearColor			( const Rgba& clearColor );

	void SetCurrentTexture	( Texture const * newTexture );
	void DrawMesh			( Mesh const &mesh, const Matrix44 & modelMatrix = Matrix44() );
	void BindMeshToProgram	( ShaderProgram const *shaderProgram, Mesh const *mesh );

	template <typename VERTTYPE>
	void DrawMeshImmediate( const VERTTYPE* vertexBuffer, unsigned int numVertexes, ePrimitiveType primitiveType, const Matrix44 &modelMatrix = Matrix44() )
	{
		m_immediateMesh->SetVertices <VERTTYPE>( numVertexes, vertexBuffer );
		m_immediateMesh->SetDrawInstruction( primitiveType, false, 0, numVertexes );
		DrawMesh( *m_immediateMesh, modelMatrix );
	}

	template <typename VERTTYPE>
	void DrawMeshImmediate( const VERTTYPE* vertexBuffer, unsigned int numVertexes, unsigned int* indexBuffer, int numIndexes, ePrimitiveType primitiveType, const Matrix44 &modelMatrix = Matrix44() )
	{
		m_immediateMesh->SetVertices  <VERTTYPE>( numVertexes, vertexBuffer );
		m_immediateMesh->SetIndices					( numIndexes, indexBuffer );
		m_immediateMesh->SetDrawInstruction( primitiveType, true, 0, numIndexes );
		DrawMesh( *m_immediateMesh, modelMatrix );
	}

	void DrawLine			( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness );
	void DrawFromVertexArray( const Vector2  vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale );
	void DrawPolygon		( const Vector2& center, float radius, float sides, float orientationAngle );
	void DrawDottedPolygon	( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color );
	
	void DrawTexturedCube	( const Vector3& center, const Vector3& dimensions, // width, height, depth
								const Rgba&  color		= RGBA_WHITE_COLOR,
								const Texture* texture	= nullptr,				// if nullptr, draws a solid cube
								const AABB2& uv_top		= AABB2::ONE_BY_ONE, 
								const AABB2& uv_side	= AABB2::ONE_BY_ONE, 
								const AABB2& uv_bottom	= AABB2::ONE_BY_ONE,
								const Texture* secondaryTexture = nullptr );

	void BindTexture2D		( const Texture& theTexture, unsigned int bindIndex = 0 );

	void DrawAABB			( const AABB2& bounds, const Rgba& color );
	void DrawTexturedAABB	( const AABB2& bounds, const Texture& texture, 
							  const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, 
							  const Rgba& tint );
	void DrawTexturedAABB	( const Matrix44 &transformMatrix,	const AABB2& bounds,	
							  const Texture& texture, 			const Vector2& texCoordsAtMins,	const Vector2& texCoordsAtMaxs, 
							  const Rgba& tint );
	void DrawTexturedAABBArray ( const Vertex_3DPCU* vertexes, int numVertexes, const Texture& texture );

	void DrawText2D		    ( const Vector2& drawMins, const std::string& asciiText,
							  float cellHeight, const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextInBox2D    ( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
							  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr, 
							  TextDrawMode drawMode = TEXT_DRAW_OVERRUN );

	Texture*		CreateOrGetTexture	 ( const std::string& pathToImage );
 	BitmapFont*		CreateOrGetBitmapFont( const char* bitmapFontName );

	static GLenum	ToGLCompare( eCompare compare );
	void			EnableDepth( eCompare compare, bool should_write );
	void			DisableDepth();
	void			ClearDepth( float depth = 1.0f );

	static void			SetCurrentCameraTo( Camera* newCamera );
	static Texture*		CreateRenderTarget( unsigned int width, unsigned int height, eTextureFormat fmt = TEXTURE_FORMAT_RGBA8 );
	static Texture*		CreateDepthStencilTarget( unsigned int width, unsigned int height );
	static Texture*		GetDefaultColorTarget();
	static Texture*		GetDefaultDepthTarget();
	

	GLenum		GetAsOpenGLPrimitiveType	( ePrimitiveType	inPrimitive ) const;
	GLenum		GetAsOpenGLDataType			( eRenderDataType	inDataType	) const;

private:
	static void PostStartup();
	bool		CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src );

	bool		findTextureFromPool		 ( const std::string& pathToImage , Texture* &foundTexture );
	bool		findBitmapFontFromPool	 ( const std::string& nameOfFont , BitmapFont* &foundFont );
	bool		FindShaderProgramFromPool( const std::string& nameOfShaderProgram, ShaderProgram* &foundShader );

	void DrawTextAsOverrun		( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextAsShrinkToFit	( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );
	void DrawTextAsWordWrap		( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
								  const Rgba& tint = RGBA_WHITE_COLOR, const BitmapFont* font = nullptr );

	void DrawCube				( const Vector3& center, const Vector3& dimensions, // width, height, depth
								  const Rgba&  color		= RGBA_WHITE_COLOR,
								  const AABB2& uv_top		= AABB2::ONE_BY_ONE, 
								  const AABB2& uv_side		= AABB2::ONE_BY_ONE, 
								  const AABB2& uv_bottom	= AABB2::ONE_BY_ONE );
};