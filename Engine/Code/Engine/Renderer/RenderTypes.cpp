#pragma once
#include "RenderTypes.hpp"

int openGLTextureFormat[ NUM_TEXTURE_FORMATS ] =
{
	GL_RGBA8,
	GL_DEPTH_STENCIL
};

int openGLPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
};

int openGLRenderDataTypes[ NUM_RENDER_DATA_TYPES ] =
{
	GL_FLOAT,
	GL_UNSIGNED_BYTE
};

int openGLCompareTypes[ NUM_COMPARES ] =
{
	GL_NEVER,
	GL_LESS,
	GL_LEQUAL,
	GL_GREATER,
	GL_GEQUAL,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_ALWAYS
};

int openGLFillModes[ NUM_FILL_MODES ] =
{
	GL_FILL,
	GL_LINE
};

int openGLCullModes[ NUM_CULLMODES ] =
{
	GL_BACK,
	GL_FRONT,
	GL_NONE
};

int openGLWindOrders[ NUM_WIND_ORDERS ] =
{
	GL_CW,
	GL_CCW
};

int openGLBlendOperations[ NUM_BLEND_OPERATIONS ] =
{
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

int openGLBlendFactors[ NUM_BLEND_FACTORS ] =
{
	GL_ONE,
	GL_ZERO,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR
};

int openGLTextureCubeSide[ NUM_TEXCUBE_SIDES ] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

int openGLPixelFormat[ NUM_PIXEL_FORMATS ] =
{
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_BGR,
	GL_RGBA,
	GL_BGRA,
	GL_DEPTH_COMPONENT,
	GL_STENCIL_INDEX
};

GLenum GetAsOpenGLTextureFormat( eTextureFormat inTexFormat )
{
	return openGLTextureFormat[ inTexFormat ];
}

GLenum GetAsOpenGLPrimitiveType( ePrimitiveType inPrimitive )
{
	return openGLPrimitiveTypes[ inPrimitive ];
}

GLenum GetAsOpenGLDataType( eRenderDataType inDataType )
{
	return openGLRenderDataTypes[ inDataType ];
}

GLenum GetAsOpenGLDataType( eCompare inDataType )
{
	return openGLCompareTypes[ inDataType ];
}

GLenum GetAsOpenGLDataType( eFillMode inDataType )
{
	return openGLFillModes[ inDataType ];
}

GLenum GetAsOpenGLDataType( eCullMode inDataType )
{
	return openGLCullModes[ inDataType ];
}

GLenum GetAsOpenGLDataType( eWindOrder inDataType )
{
	return openGLWindOrders[ inDataType ];
}

GLenum GetAsOpenGLDataType( eBlendOperation inDataType )
{
	return openGLBlendOperations[ inDataType ];
}

GLenum GetAsOpenGLDataType( eBlendFactor inDataType )
{
	return openGLBlendFactors[ inDataType ];
}

GLenum GetAsOpenGLTextureCubeSide( eTextureCubeSide inTexCubeSide )
{
	return openGLTextureCubeSide[ inTexCubeSide ];
}

GLenum GetAsOpenGLPixelFormat( ePixelFormat inPixelFormats )
{
	return openGLPixelFormat[ inPixelFormats ];
}
