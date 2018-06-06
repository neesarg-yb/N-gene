#pragma once
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

class ShaderProgram;

struct RenderState
{
	// Raster State Control
	eCullMode		m_cullMode			= CULLMODE_BACK;
	eFillMode		m_fillMode			= FRONT_AND_BACK_FILL;
	eWindOrder		m_frontFace			= WIND_COUNTER_CLOCKWISE;

	// Depth State Control
	bool			m_depthEnabled		= true;
	eCompare		m_depthCompare		= COMPARE_LESS;
	bool			m_depthWrite		= true;

	// Blend
	bool			m_blendEnabled		= true;
	eBlendOperation m_colorBlendOp		= BLEND_OP_ADD;
	eBlendFactor	m_colorSrcFactor	= BLEND_FACTOR_ONE;
	eBlendFactor	m_colorDstFactor	= BLEND_FACTOR_ZERO;

	eBlendOperation m_alphaBlendOp		= BLEND_OP_ADD;
	eBlendFactor	m_alphaSrcFactor	= BLEND_FACTOR_ONE;
	eBlendFactor	m_alphaDstFactor	= BLEND_FACTOR_ONE;
};

class Shader
{
public:
	static Shader* CreateNewFromFile( std::string const &shaderPath );

public:
	 Shader( XMLElement const &shaderRoot );
	~Shader();

public:
	void SetShaderProgram( ShaderProgram * shaderProgram );

	void EnableBlending	 ( eBlendOperation op, eBlendFactor src, eBlendFactor dst );
	void DisableBlending ();
						 
	void SetDepth		 ( eCompare compare, bool write );
	void DisableDepth	 ();
						 
	void SetCullMode	 ( eCullMode  cullMode );
	void SetFillMode	 ( eFillMode  fillMode );
	void SetFrontFace	 ( eWindOrder windOrder );

public:
	std::string		 m_name;
	ShaderProgram	*m_program;
	RenderState		 m_renderState;

	// Here goes the layer and queue info.
	unsigned int	 m_layer			= 0;
	bool			 m_isAlphaQueueType = false;		// Queue: ALPHA || OPAQUE

private:
	eCullMode		ParseXmlAttribute( XMLElement const &element, char const *attributeName, eCullMode			const &defaultValue );
	eFillMode		ParseXmlAttribute( XMLElement const &element, char const *attributeName, eFillMode			const &defaultValue );
	eWindOrder		ParseXmlAttribute( XMLElement const &element, char const *attributeName, eWindOrder			const &defaultValue );
	eCompare		ParseXmlAttribute( XMLElement const &element, char const *attributeName, eCompare			const &defaultValue );
	eBlendOperation	ParseXmlAttribute( XMLElement const &element, char const *attributeName, eBlendOperation	const &defaultValue );
	eBlendFactor	ParseXmlAttribute( XMLElement const &element, char const *attributeName, eBlendFactor		const &defaultValue );
};