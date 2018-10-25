#pragma once
#include "DebugRenderObject.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Shader.hpp"


bool DebugRenderObject::s_isDebugRenderingEnabled = true;
void DebugRenderObject::ToggleDebugRendering( Command& cmd )
{
	std::string trueOrFalse = cmd.GetNextString();

	if( trueOrFalse == "false" )
		s_isDebugRenderingEnabled = false;
	else
		s_isDebugRenderingEnabled = true;
}

Shader* DebugRenderObject::s_debugShader = nullptr;

DebugRenderObject::DebugRenderObject( float lifetime, Renderer &renderer, Camera &camera, Matrix44 modelMatrix, Mesh const *newMesh, Texture const *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode renderMode /* = DEBUG_RENDER_USE_DEPTH */, eFillMode polygonMode /* = FRONT_AND_BACK_FILL */ )
	: m_lifetime( lifetime )
	, m_renderer( renderer )
	, m_modelMatrix( modelMatrix )
	, m_camera( camera )
	, m_mesh( newMesh )
	, m_texture( texture )
	, m_startColor( startColor )
	, m_endColor( endColor )
	, m_renderMode( renderMode )
	, m_polygonMode( polygonMode )
{
	if( s_debugShader == nullptr )
		s_debugShader = m_renderer.CreateOrGetShader( "debug" );

	// Command Register
	CommandRegister( "enable_debug", DebugRenderObject::ToggleDebugRendering );
}

DebugRenderObject::~DebugRenderObject()
{
	delete m_mesh;
}

void DebugRenderObject::Update( float deltaSeconds )
{
	m_timeElapsed += deltaSeconds;

	if( m_timeElapsed > m_lifetime )
		m_deleteMe = true;
}

void DebugRenderObject::Render() const
{
	if( s_isDebugRenderingEnabled == false )
		return;

	m_renderer.UseShader( s_debugShader );

	// Set the PolygonMode
	switch (m_polygonMode)
	{
	case FRONT_AND_BACK_FILL:
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		break;

	case FRONT_AND_BACK_LINE:
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		break;
	}

	// Set the DepthMode
	switch (m_renderMode)
	{
	case DEBUG_RENDER_IGNORE_DEPTH:
		m_renderer.EnableDepth( COMPARE_ALWAYS, true );
		break;
	case DEBUG_RENDER_USE_DEPTH:
		m_renderer.EnableDepth( COMPARE_LESS, true ); 
		break;
	default:
		m_renderer.EnableDepth( COMPARE_LESS, true ); 
		break;
	}

	// Color Lerp
	float lerpFraction = ClampFloat01( m_timeElapsed / m_lifetime );
	Rgba  debugColorLerp = Interpolate( m_startColor, m_endColor, lerpFraction );
	m_renderer.SetUniform( "COLORLERP", debugColorLerp );

	m_renderer.SetCurrentDiffuseTexture( m_texture );
	m_renderer.BindCamera( &m_camera );
	m_renderer.DrawMesh( *m_mesh, m_modelMatrix );


	// Reset after draw..
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}