#pragma once
#include <vector>

class Light;
class Camera;
class Renderable;

class Scene
{
public:
	void AddLight		( Light &light );
	void AddCamera		( Camera &camera );
	void AddRenderable	( Renderable &renderable );

public:
	std::vector< Light* >		m_lights;
	std::vector< Camera* >		m_cameras;
	std::vector< Renderable* >	m_renderables;
};