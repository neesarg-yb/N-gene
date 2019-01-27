#pragma once
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Core/EngineCommon.hpp"

struct LightData
{
	Vector4		colorAndIntensity;				// alpha is intensity
	
	// WARNING: IT SHOULD ALWAY BE VECTOR3 & THEN FLOAT
	Vector3		position;
	float		padding_01;

	Vector3		direction;
	float		directionFactor;				// 1: directionLight, 0: pointLight

	Vector3		attenuation;
	float		dotInnerAngle;
	
	Vector3		padding_02;
	float		dotOuterAngle;

	Vector3		padding_03;
	float		isUsingShadow;

	Matrix44	viewProjectionMatrix;
};

struct UBOLightsBlock
{
	Vector4		ambientLight;
	LightData	lights[ MAX_LIGHTS ];
};

struct UBOObjectLightData
{
	float	specularAmount;
	float	specularPower;
	Vector2	padding_00;
};

struct UBOTimeData
{
	float gameDeltaTime		= 0.f;
	float gameTotalTime		= 0.f;
	float systemDeltaTime	= 0.f;
	float systemTotalTime	= 0.f;
};

struct UBOCameraMatrices
{
	Vector3		padding_04;
	float		usesShadowMap;

	Matrix44	viewMatrix;
	Matrix44	projectionMatrix;
};

enum eTextureFormat
{
	TEXTURE_FORMAT_RGBA8 = 0,		// GL_RGBA8, default color format
	TEXTURE_FORMAT_D24S8,			// GL_DEPTH_STENCIL
	NUM_TEXTURE_FORMATS
};
GLenum GetAsOpenGLTextureFormat( eTextureFormat inTexFormat );

enum eTextDrawMode
{
	TEXT_DRAW_OVERRUN = 0,
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP,
	NUM_TEXT_DRAW_MODES
};

enum eDebugRenderMode
{
	DEBUG_RENDER_IGNORE_DEPTH = 0,	// will always draw and be visible
	DEBUG_RENDER_USE_DEPTH,			// draw using normal depth rules
	DEBUG_RENDER_HIDDEN,			// only draws if it would be hidden by depth
	DEBUG_RENDER_XRAY,				// always draws, but hidden area will be drawn differently
	NUM_DEBUG_RENDER_MODES
};

enum ePrimitiveType
{
	PRIMITIVE_POINTS = 0,			// in OpenGL, for example, this becomes GL_POINTS
	PRIMITIVE_LINES,				// in OpenGL, for example, this becomes GL_LINES
	PRIMITIVE_TRIANGES,				// in OpenGL, for example, this becomes GL_TRIANGLES
	NUM_PRIMITIVE_TYPES
};
GLenum GetAsOpenGLPrimitiveType( ePrimitiveType inPrimitive );

enum eCompare
{
	COMPARE_NEVER = 0,				// GL_NEVER
	COMPARE_LESS,					// GL_LESS
	COMPARE_LEQUAL,					// GL_LEQUAL
	COMPARE_GREATER,				// GL_GREATER
	COMPARE_GEQUAL,					// GL_GEQUAL
	COMPARE_EQUAL,					// GL_EQUAL
	COMPARE_NOT_EQUAL,				// GL_NOTEQUAL
	COMPARE_ALWAYS,					// GL_ALWAYS
	NUM_COMPARES
};
GLenum GetAsOpenGLDataType( eCompare inDataType );


enum eRenderDataType
{
	RDT_FLOAT = 0,					// GL_FLOAT
	RDT_UNSIGNED_INT,				// GL_UNSIGNED_BYTE
	NUM_RENDER_DATA_TYPES
};
GLenum GetAsOpenGLDataType( eRenderDataType inDataType );

enum eFillMode
{
	FRONT_AND_BACK_FILL = 0,		// GL_FILL     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL )
	FRONT_AND_BACK_LINE,			// GL_LINE     glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )
	NUM_FILL_MODES
};
GLenum GetAsOpenGLDataType( eFillMode inDataType );

enum eCullMode
{
	CULLMODE_BACK = 0,				// GL_BACK     glEnable(GL_CULL_FACE); glCullFace(GL_BACK); 
	CULLMODE_FRONT,					// GL_FRONT    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); 
	CULLMODE_NONE,					// GL_NONE     glDisable(GL_CULL_FACE)
	NUM_CULLMODES
};
GLenum GetAsOpenGLDataType( eCullMode inDataType );

enum eWindOrder
{
	WIND_CLOCKWISE = 0,				// GL_CW       glFrontFace( GL_CW ); 
	WIND_COUNTER_CLOCKWISE,			// GL_CCW      glFrontFace( GL_CCW ); 
	NUM_WIND_ORDERS
};
GLenum GetAsOpenGLDataType( eWindOrder inDataType );

enum eBlendOperation
{
	BLEND_OP_ADD = 0,				// GL_FUNC_ADD					glBlendFunci( GL_FUNC_ADD );
	BLEND_OP_SUB,					// GL_FUNC_SUBTRACT				glBlendFunci( GL_FUNC_SUBTRACT );
	BLEND_OP_REV_SUB,				// GL_FUNC_REVERSE_SUBTRACT		glBlendFunci( GL_FUNC_REVERSE_SUBTRACT );
	BLEND_OP_MIN,					// GL_MIN						glBlendFunci( GL_MIN );
	BLEND_OP_MAX,					// GL_MAX						glBlendFunci( GL_MAX );
	NUM_BLEND_OPERATIONS
};
GLenum GetAsOpenGLDataType( eBlendOperation inDataType );

enum eBlendFactor
{
	BLEND_FACTOR_ONE = 0,			// GL_ONE						glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_ZERO,				// GL_ZERO						glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_SRC_COLOR,			// GL_SRC_COLOR					glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_INV_SRC_COLOR,		// GL_ONE_MINUS_SRC_COLOR		glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_SRC_ALPHA,			// GL_SRC_ALPHA					glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_INV_SRC_ALPHA,		// GL_ONE_MINUS_SRC_ALPHA		glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_DEST_COLOR,		// GL_DST_COLOR					glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_INV_DEST_COLOR,	// GL_ONE_MINUS_DST_COLOR		glBlendFunc( srcFactor, destFactor );	
	BLEND_FACTOR_DEST_ALPHA,		// GL_DST_ALPHA					glBlendFunc( srcFactor, destFactor );	
	BLEND_FACTOR_INV_DEST_ALPHA,	// GL_ONE_MINUS_DST_ALPHA		glBlendFunc( srcFactor, destFactor );	
	BLEND_FACTOR_CONSTANT,			// GL_CONSTANT_COLOR			glBlendFunc( srcFactor, destFactor );
	BLEND_FACTOR_INV_CONSTANT,		// GL_ONE_MINUS_CONSTANT_COLOR	glBlendFunc( srcFactor, destFactor );
	NUM_BLEND_FACTORS
};
GLenum GetAsOpenGLDataType( eBlendFactor inDataType );

enum eTextureCubeSide
{
	TEXCUBE_RIGHT = 0,				// GL_TEXTURE_CUBE_MAP_POSITIVE_X
	TEXCUBE_LEFT,					// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	TEXCUBE_TOP,					// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	TEXCUBE_BOTTOM,					// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	TEXCUBE_FRONT,					// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	TEXCUBE_BACK,					// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	NUM_TEXCUBE_SIDES
};
GLenum GetAsOpenGLTextureCubeSide( eTextureCubeSide inTexCubeSide );

enum ePixelFormat
{
	PIXEL_FORMAT_RED = 0,				// GL_RED, 
	PIXEL_FORMAT_RG,					// GL_RG, 
	PIXEL_FORMAT_RGB,					// GL_RGB, 
	PIXEL_FORMAT_BGR,					// GL_BGR, 
	PIXEL_FORMAT_RGBA,					// GL_RGBA, 
	PIXEL_FORMAT_BGRA,					// GL_BGRA, 
	PIXEL_FORMAT_DEPTH_COMPONENT,		// GL_DEPTH_COMPONENT, 
	PIXEL_FORMAT_STENCIL_INDEX,			// GL_STENCIL_INDEX
	NUM_PIXEL_FORMATS
};
GLenum GetAsOpenGLPixelFormat( ePixelFormat inPixelFormats );