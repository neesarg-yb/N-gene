#include "Scene.hpp"

void Scene::AddLight( Light &light )
{
	// If light already exists, don't add it
	for each( Light *existingLight in m_lights )
	{
		if( existingLight == &light )
			return;
	}

	m_lights.push_back( &light );
}

void Scene::AddCamera( Camera &camera )
{
	// If camera already exists, don't add it
	for each( Camera *existingCamera in m_cameras )
	{
		if( existingCamera == &camera )
			return;
	}

	m_cameras.push_back( &camera );
}

void Scene::AddRenderable( Renderable &renderable )
{
	m_renderables.push_back( &renderable );
}
