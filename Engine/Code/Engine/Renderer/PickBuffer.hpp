#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Scene;
class Camera;
class Renderer;
class Texture;
class Material;

class PickBuffer
{
public:
	 PickBuffer( Renderer &activeRenderer );
	~PickBuffer();

public:
	IntVector2	m_bufferDimension	= IntVector2::ZERO;

private:
	Renderer&	m_activeRenderer;
	Camera*		m_camera			= nullptr;
	Texture*	m_pickTarget		= nullptr;
	Texture*	m_depthTarget		= nullptr;
	Material*	m_pickMaterial		= nullptr;

public:
	void GeneratePickBuffer( Camera &activeCamera, Scene &currentScene );
	uint GetHandle( Vector2 mousePosition );

private:
	void ResizePickTargets( uint width, uint height );
};