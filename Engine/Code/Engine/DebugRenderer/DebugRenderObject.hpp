#pragma once
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/Matrix44.hpp"

class Renderer;
class Camera;
class Texture;
class Command;

enum eDebugRenderMode
{
	DEBUG_RENDER_IGNORE_DEPTH,		// will always draw and be visible
	DEBUG_RENDER_USE_DEPTH,			// draw using normal depth rules
	DEBUG_RENDER_HIDDEN,			// only draws if it would be hidden by depth
	DEBUG_RENDER_XRAY				// always draws, but hidden area will be drawn differently
};

enum ePolygonMode
{
	FRONT_AND_BACK_FILL,
	FRONT_AND_BACK_LINE
};

class DebugRenderObject
{
public:
	 DebugRenderObject( float lifetime, Renderer &renderer, Camera &camera, Matrix44 modelMatrix, Mesh const *newMesh, Texture const *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode renderMode = DEBUG_RENDER_USE_DEPTH, ePolygonMode polygonMode = FRONT_AND_BACK_FILL );
	~DebugRenderObject();

public:
	static bool	s_isDebugRenderingEnabled;
	static void ToggleDebugRendering( Command& cmd );			// Toggle it ON/OFF

public:
	bool				m_deleteMe		= false;
	float				m_lifetime		= 0.f;

	Renderer			&m_renderer;
	Camera				&m_camera;
	Matrix44			m_modelMatrix;	// Defaults to the Identity
	Mesh const			*m_mesh			= nullptr;
	Texture	const		*m_texture		= nullptr;
	Rgba				m_startColor	= RGBA_WHITE_COLOR;
	Rgba				m_endColor		= RGBA_RED_COLOR;
	eDebugRenderMode	m_renderMode	= DEBUG_RENDER_USE_DEPTH;
	ePolygonMode		m_polygonMode	= FRONT_AND_BACK_FILL;

public:
	void Update( float deltaSeconds );
	void Render() const;
};