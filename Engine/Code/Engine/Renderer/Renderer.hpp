#pragma once

#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/Material.hpp"

/*
	UBO Binding Slots:
		1 = uboTimeClock
		2 = uboCamera
		3 = uboObjectLightData
		4 = uboLightsBlock

	RENDER TARGETS:
		0 = Color Target
		1 = Bloom Texture ( from Camera )
		3 = Pick Buffer Target
*/

struct LoadedTexturesData {
	std::string pathToImage;
	Texture* texture;

	LoadedTexturesData( std::string path, Texture* newTexture) {
		pathToImage = path;
		texture = newTexture;
	}
};

class TextureCube;

class Renderer
{
public:
	static Camera*		s_current_camera;

private:
	static Renderer*	s_renderer;

	Mesh*					m_immediateMesh				= nullptr;
	RenderBuffer*			m_temp_render_buffer		= nullptr;
	UniformBuffer*			m_timeUBO					= nullptr;
	UniformBuffer*			m_objectLightDataUBO		= nullptr;
	UniformBuffer*			m_lightsBlockUBO			= nullptr;
	static Camera*			s_default_camera;
	static Sampler*			s_defaultNearestSampler;
	static Sampler*			s_defaultLinearSampler;
	static Texture*			s_defaultColorTarget;
	static Texture*			s_defaultDepthTarget;
	const  Texture*			m_defaultWhiteTexture		= nullptr;
	const  Texture*			m_secondaryTexture			= nullptr;
	const  Texture*			m_defaultNormalTexture		= nullptr;
	const  Texture*			m_defaultEmissiveTexture	= nullptr;

	static Camera*			s_effectsCamera;
	static Texture*			s_sketchColorTarget;
	static Texture*			s_effectCurrentTarget;
	static Texture*			s_effectCurrentSource;


public:
	Shader const *m_defaultShader = nullptr;
	Shader const *m_currentShader = nullptr;
	static GLuint  s_default_vao;

	// How many UBOs
	static unsigned int			s_maxConstantBufferBindings;
	static unsigned int			s_maxConstantBufferSize;
	static unsigned int const	s_maxLights = MAX_LIGHTS;

	std::vector< LoadedTexturesData >		m_texturePool;
	std::map< std::string , BitmapFont* >	m_bitmapFontPool;
	std::map< std::string, Shader* >		m_shaderPool;

	Vector2 framesBottomLeft;
	Vector2 framesTopRight;
	Rgba	defaultInkColor;
	Rgba	defaultColor;
	float	defaultDrawingThickness;
	
	 Renderer();
	~Renderer();

	static Renderer*GetInstance();																						// Returns the last created Renderer instance
	
	void			BeginFrame();
	void			EndFrame();

	static bool		RendererStartup( void* hwnd );
	static void		RendererShutdown();
	static void		GLShutdown();

	Texture*		CreateOrGetTexture( const std::string& pathToImage );
	BitmapFont*		CreateOrGetBitmapFont( const char* bitmapFontName );												// bitmapFontName = default, if it is default.png
	Shader*			CreateOrGetShader( const char* shaderfileName );													// shaderFileName = default, if it is default.shader

	void SetAmbientLight( Vector4 normalizedAmbientLight );																// (x,y,z) = (r,g,b) & w = intensity_clamped_to_01
	void SetAmbientLight( float intensity, Rgba const &color );															// intensity [ 0.f, 1.f ]
	void DisableAllLights();
	void EnableLight( unsigned int idx, Light const &theLight );
	void SetSpecularConstants( float specAmount, float specPower );
	void UpdateLightUBOs();																								// ObjectLightData & LightsBlock

	void LoadAllInbuiltShaders();																						// Also adds it to m_shaderProgramPool
	void UseShader( Shader const *useShader );
	void BindRenderState( RenderState const &renderState );
	void BindMaterialForShaderIndex( Material &material, uint shaderIndex = 0 );

	void SetUniform( char const *name, uint unsignedInt );
	void SetUniform( char const *name, float flt );
	void SetUniform( char const *name, Vector3 const &vct );
	void SetUniform( char const *name, Vector4 const &vct );
	void SetUniform( char const *name, Rgba const &clr );
	void SetUniform( char const *name, Matrix44 const &mat44 );
	
	void UpdateTime( float gameDeltaSeconds, float systemDeltaSeconds );
		 
	void ClearScreen( const Rgba& clearColor );
	void ClearColor( const Rgba& clearColor );
	void EnableDepth( eCompare compare, bool should_write );
	void DisableDepth();
	void ClearDepth( float depth = 1.0f );
	void SetCullingMode( eCullMode newCullMode );

	void ApplyEffect( Shader* effectShader );
	void EndEffect();

	void SetCurrentDiffuseTexture	( Texture const * newTexture );
	void SetCurrentNormalTexture	( Texture const * newNormalTexture );
	void SetCurrentEmissiveTexture	( Texture const * newEmessiveTexture );

	void BindCamera			( Camera *camera );
	void BindMeshToProgram	( ShaderProgram const *shaderProgram, Mesh const *mesh );
	void BindTexture2D		( unsigned int bindIndex, const Texture& theTexture, Sampler const *theSampler = nullptr );
	void BindTextureCube	( unsigned int bindIndex, const TextureCube& texCube, Sampler const *theSampler = nullptr );

	void DrawTexturedCube	( const Vector3& center, const Vector3& dimensions,		// width, height, depth
							  const Rgba&  color		= RGBA_WHITE_COLOR,
							  const Texture* texture	= nullptr,					// if nullptr, draws a solid cube
							  const AABB2& uv_top		= AABB2::ONE_BY_ONE, 
							  const AABB2& uv_side	= AABB2::ONE_BY_ONE, 
							  const AABB2& uv_bottom	= AABB2::ONE_BY_ONE,
							  const Texture* secondaryTexture = nullptr );
	void DrawAABB			( const AABB2& bounds, const Rgba& color );
	void DrawTexturedAABB	( const AABB2& bounds, const Texture& texture, 
							  const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, 
							  const Rgba& tint );
	void DrawTexturedAABB	( const Matrix44 &transformMatrix,	
							  const Texture& texture, 			const Vector2& texCoordsAtMins,	const Vector2& texCoordsAtMaxs, 
							  const Rgba& tint );
	void DrawText2D		    ( const Vector2& drawMins,	const std::string& asciiText,
							  float cellHeight,			const Rgba& tint = RGBA_WHITE_COLOR,  const BitmapFont* font = nullptr );
	void DrawTextInBox2D    ( const std::string& asciiText,			const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, 
							  const Rgba& tint = RGBA_WHITE_COLOR,	const BitmapFont* font = nullptr, 
							  eTextDrawMode drawMode = TEXT_DRAW_OVERRUN );

	void DrawMesh			( Mesh const &mesh, const Matrix44 & modelMatrix = Matrix44() );

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

	static Sampler const*	GetDefaultSampler( eSamplerType type = SAMPLER_NEAREST );
	static Texture*			GetDefaultColorTarget();
	static Texture*			GetDefaultDepthTarget();
	static Texture*			CreateRenderTarget( unsigned int width, unsigned int height, eTextureFormat fmt = TEXTURE_FORMAT_RGBA8 );
	static Texture*			CreateDepthStencilTarget( unsigned int width, unsigned int height );
	static Vector3			GetDrawPositionUsingAnchorPoint( const Vector3& position, const Vector3& dimensions, const Vector3& anchorPoint = Vector3::ZERO );	// AnchorPoints: Bottom = (0, -1, 0), Left = (-1, 0, 0), Far = (1, 0, 0)

private:
	static void PostStartup();
	bool		CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src );

	bool		findTextureFromPool		 ( const std::string& pathToImage , Texture* &foundTexture );
	bool		findBitmapFontFromPool	 ( const std::string& nameOfFont , BitmapFont* &foundFont );
	bool		FindShaderFromPool		 ( const std::string& nameOfShaderProgram, Shader* &foundShader );

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