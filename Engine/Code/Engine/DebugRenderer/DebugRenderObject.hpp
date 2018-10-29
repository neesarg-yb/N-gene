#pragma once
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

class Renderer;
class Camera;
class Texture;
class Command;
class Shader;

enum eDebugRenderCameraType
{
	DEBUG_CAMERA_2D = 0,
	DEBUG_CAMERA_3D,
	NUM_DEBUG_CAMERAS
};

class DebugRenderObject
{
public:
	 DebugRenderObject( float lifetime, Renderer &renderer, eDebugRenderCameraType cameraType, Matrix44 modelMatrix, Mesh const *newMesh, Texture const *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode renderMode = DEBUG_RENDER_USE_DEPTH, eFillMode polygonMode = FRONT_AND_BACK_FILL );
	~DebugRenderObject();

public:
	static Shader*	s_debugShader;
	static bool		s_isDebugRenderingEnabled;
	static void ToggleDebugRendering( Command& cmd );			// Toggle it ON/OFF

public:
	bool				m_deleteMe		= false;
	float const			m_lifetime;
	float				m_timeElapsed	= 0.f;

	Renderer				&m_renderer;
	eDebugRenderCameraType	 m_cameraType	= DEBUG_CAMERA_3D;
	Matrix44				 m_modelMatrix;	// Defaults to the Identity
	Mesh const				*m_mesh			= nullptr;
	Texture	const			*m_texture		= nullptr;
	Rgba					 m_startColor	= RGBA_WHITE_COLOR;
	Rgba					 m_endColor		= RGBA_RED_COLOR;
	eDebugRenderMode		 m_renderMode	= DEBUG_RENDER_USE_DEPTH;
	eFillMode				 m_polygonMode	= FRONT_AND_BACK_FILL;

public:
	void Update( float deltaSeconds );
	void Render( Camera &camera ) const;
};
