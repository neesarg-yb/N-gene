#pragma once
#include "Engine/File/File.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

using namespace tinyxml2;

Shader* Shader::CreateNewFromFile( std::string const &shaderPath )
{
	// Loading: Shader XML
	XMLDocument shaderDoc;
	shaderDoc.LoadFile( shaderPath.c_str() );
	const XMLElement* shaderDefRoot = shaderDoc.RootElement();
	GUARANTEE_OR_DIE( shaderDefRoot != nullptr, "Error: Shader() couldn't find the rootElement of XML file..!" );
	return new Shader( *shaderDefRoot );
}

Shader::Shader( XMLElement const & shaderRoot )
{
	// Root Element
	m_name = ::ParseXmlAttribute( shaderRoot, "name", std::string("") );
	GUARANTEE_RECOVERABLE( m_name != "", "Error, Shader: name can't be empty!" );

	m_renderState.m_cullMode	=	  ParseXmlAttribute( shaderRoot, "cull",		m_renderState.m_cullMode );
	m_renderState.m_fillMode	=	  ParseXmlAttribute( shaderRoot, "fill",		m_renderState.m_fillMode );
	m_renderState.m_frontFace	=	  ParseXmlAttribute( shaderRoot, "frontface",	m_renderState.m_frontFace );
	
	m_layer					= (unsigned int)::ParseXmlAttribute( shaderRoot, "layer", (int)m_layer );
	std::string queueType	=				::ParseXmlAttribute( shaderRoot, "queue", "opaque" );
	m_isAlphaQueueType		= (queueType == "alpha") ? true : false;

	// Program Element
	XMLElement const *programElement = shaderRoot.FirstChildElement( "program" );
	GUARANTEE_OR_DIE( programElement != nullptr, "Error, Shader: No program element found.." );


	// Define tags
	std::string defineTagsCombined	= ::ParseXmlAttribute( *programElement, "define", std::string("") );
	std::vector< std::string > tagsSaperated;
	if( defineTagsCombined != "" )
		tagsSaperated = SplitIntoStringsByDelimiter( defineTagsCombined, ';' );

	
	// Vertex and Fragment ShaderPrograms
	XMLElement const *vertexElement	= programElement->FirstChildElement( "vertex" );
	GUARANTEE_OR_DIE( vertexElement != nullptr, "Error, Shader: No vertex element found..");
	XMLElement const *fragmentElemnt = programElement->FirstChildElement( "fragment" );
	GUARANTEE_OR_DIE( fragmentElemnt != nullptr, "Error, Shader: No fragement element found.." );

	std::string vertexShaderPath	= ::ParseXmlAttribute( *vertexElement, "file", "" );
	std::string fragmentShaderPath	= ::ParseXmlAttribute( *fragmentElemnt, "file", "" );

	const char* vertexShaderText	= (const char*) FileReadToNewBuffer( vertexShaderPath.c_str() );
	const char* fragmentShaderText	= (const char*) FileReadToNewBuffer( fragmentShaderPath.c_str() );
	GUARANTEE_OR_DIE( vertexShaderText != nullptr && fragmentShaderText != nullptr, "Error: Shader can't open the vertex or fragment shader file.." );

	std::string vertexShaderString( vertexShaderText );
	std::string fragmentShaderString( fragmentShaderText );
	delete vertexShaderText;	vertexShaderText = nullptr;
	delete fragmentShaderText;	fragmentShaderText = nullptr;

	// Attach tags in front of the ShaderPrograms
	for (unsigned int i = 0; i < tagsSaperated.size(); i++)
	{
		std::string thisDefineTag = tagsSaperated[i];
		size_t vertStartLoc = vertexShaderString.find("#version 420 core");
		size_t fragStartLoc = fragmentShaderString.find("#version 420 core");

		vertexShaderString.insert	( vertStartLoc + 1 + 16, "\n #define " + thisDefineTag );
		fragmentShaderString.insert	( fragStartLoc + 1 + 16, "\n #define " + thisDefineTag );
	}


	// Create the ShaderProgram, now
	ShaderProgram *toReturn = new ShaderProgram();
	bool newShaderLoaded = toReturn->LoadFromStrings( vertexShaderString.c_str(), fragmentShaderString.c_str() );
	GUARANTEE_OR_DIE( newShaderLoaded != false, "Error, Shader: ShaderProgram failed to load from provided paths..!" );
	m_program = toReturn;


	// Depth Element
	XMLElement const *depthElement	= shaderRoot.FirstChildElement( "depth" );
	if( depthElement == nullptr )
		m_renderState.m_depthEnabled = false;
	else
	{
		m_renderState.m_depthEnabled = true;
		m_renderState.m_depthWrite	 = ::ParseXmlAttribute( *depthElement, "write", m_renderState.m_depthWrite );
		m_renderState.m_depthCompare =   ParseXmlAttribute( *depthElement, "test", m_renderState.m_depthCompare );
	}

	
	// Blend Element
	XMLElement const *blendElement	= shaderRoot.FirstChildElement( "blend" );
	if( blendElement == nullptr )
		m_renderState.m_blendEnabled = false;
	else
	{
		XMLElement const *colorElement	= blendElement->FirstChildElement( "color" );
		GUARANTEE_OR_DIE( colorElement != nullptr, "Error, Shader: color element inside blend not found.." );
		XMLElement const *alphaElement	= blendElement->FirstChildElement( "alpha" );
		GUARANTEE_OR_DIE( alphaElement != nullptr, "Error, Shader: alpha element inside blend not found.." );

		m_renderState.m_blendEnabled	= true;
		m_renderState.m_colorBlendOp	= ParseXmlAttribute( *colorElement, "op", m_renderState.m_colorBlendOp );
		m_renderState.m_colorSrcFactor	= ParseXmlAttribute( *colorElement, "src", m_renderState.m_colorSrcFactor );
		m_renderState.m_colorDstFactor	= ParseXmlAttribute( *colorElement, "dest", m_renderState.m_colorDstFactor );

		m_renderState.m_alphaBlendOp	= ParseXmlAttribute( *alphaElement, "op", m_renderState.m_alphaBlendOp );
		m_renderState.m_alphaSrcFactor	= ParseXmlAttribute( *alphaElement, "src", m_renderState.m_alphaSrcFactor );
		m_renderState.m_alphaDstFactor	= ParseXmlAttribute( *alphaElement, "dest", m_renderState.m_alphaDstFactor );
	}
}

Shader::~Shader()
{
	delete m_program;
}

void Shader::SetShaderProgram( ShaderProgram * shaderProgram )
{
	m_program = shaderProgram;
}

void Shader::EnableBlending( eBlendOperation op, eBlendFactor src, eBlendFactor dst )
{
	m_renderState.m_blendEnabled		= true;

	m_renderState.m_colorBlendOp		= op;
	m_renderState.m_colorSrcFactor		= src;
	m_renderState.m_colorDstFactor		= dst;
	
	m_renderState.m_alphaBlendOp		= op;
	m_renderState.m_alphaSrcFactor		= src;
	m_renderState.m_alphaDstFactor		= dst;
}

void Shader::DisableBlending()
{
	m_renderState.m_blendEnabled = false;
}

void Shader::SetDepth( eCompare compare, bool write )
{
	m_renderState.m_depthEnabled	= true;

	m_renderState.m_depthCompare	= compare;
	m_renderState.m_depthWrite		= write;
}

void Shader::DisableDepth()
{
	m_renderState.m_depthEnabled = false;
}

void Shader::SetCullMode( eCullMode cullMode )
{
	m_renderState.m_cullMode = cullMode;
}

void Shader::SetFillMode( eFillMode fillMode )
{
	m_renderState.m_fillMode = fillMode;
}

void Shader::SetFrontFace( eWindOrder windOrder )
{
	m_renderState.m_frontFace = windOrder;
}

eCullMode Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eCullMode const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );
	
	if( parsedString == "back" )
		return CULLMODE_BACK;
	if( parsedString == "front")
		return CULLMODE_FRONT;
	if( parsedString == "none" )
		return CULLMODE_NONE;

	return defaultValue;
}

eFillMode Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eFillMode const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );

	if ( parsedString == "solid" )
		return FRONT_AND_BACK_FILL;
	if ( parsedString == "wire" )
		return FRONT_AND_BACK_LINE;

	return defaultValue;
}

eWindOrder Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eWindOrder const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );
	
	if( parsedString == "ccw" )
		return WIND_COUNTER_CLOCKWISE;
	if( parsedString == "cw" )
		return WIND_CLOCKWISE;

	return defaultValue;
}


eCompare Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eCompare const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );
	
	if( parsedString == "less" )
		return COMPARE_LESS;
	if( parsedString == "never" )
		return COMPARE_NEVER;
	if( parsedString == "equal" )
		return COMPARE_EQUAL;
	if( parsedString == "lequal" )
		return COMPARE_LEQUAL;
	if( parsedString == "greater" )
		return COMPARE_GREATER;
	if( parsedString == "gequal" )
		return COMPARE_GEQUAL;
	if( parsedString == "not" )
		return COMPARE_NOT_EQUAL;
	if( parsedString == "always" )
		return COMPARE_ALWAYS;

	return defaultValue;
}

eBlendOperation Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eBlendOperation const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );
	
	if( parsedString == "add" )
		return BLEND_OP_ADD;
	if( parsedString == "sub" )
		return BLEND_OP_SUB;
	if( parsedString == "rev_sub" )
		return BLEND_OP_REV_SUB;
	if( parsedString == "min" )
		return BLEND_OP_MIN;
	if( parsedString == "max" )
		return BLEND_OP_MAX;

	return defaultValue;
}

eBlendFactor Shader::ParseXmlAttribute( XMLElement const &element, char const *attributeName, eBlendFactor const &defaultValue )
{
	std::string parsedString = ::ParseXmlAttribute( element, attributeName, "" );

	if( parsedString == "one" )
		return BLEND_FACTOR_ONE;
	if( parsedString == "zero" )
		return BLEND_FACTOR_ZERO;
	if( parsedString == "src_color" )
		return BLEND_FACTOR_SRC_COLOR;
	if( parsedString == "inv_src_color" )
		return BLEND_FACTOR_INV_SRC_COLOR;
	if( parsedString == "src_alpha" )
		return BLEND_FACTOR_SRC_ALPHA;
	if( parsedString == "inv_src_alpha" )
		return BLEND_FACTOR_INV_SRC_ALPHA;
	if( parsedString == "dest_color" )
		return BLEND_FACTOR_DEST_COLOR;
	if( parsedString == "inv_dest_color" )
		return BLEND_FACTOR_INV_DEST_COLOR;
	if( parsedString == "dest_alpha" )
		return BLEND_FACTOR_DEST_ALPHA;
	if( parsedString == "inv_dest_alpha" )
		return BLEND_FACTOR_INV_DEST_ALPHA;
	if( parsedString == "constant" )
		return BLEND_FACTOR_CONSTANT;
	if( parsedString == "inv_constant" )
		return BLEND_FACTOR_INV_CONSTANT;

	return defaultValue;
} 