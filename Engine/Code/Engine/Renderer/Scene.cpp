#pragma once
#include "Scene.hpp"
#include "Engine/Core/EngineCommon.hpp"

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

void Scene::RemoveRenderable( Renderable &removeMe )
{
	uint totalRenderables = m_renderables.size();

	for( uint i = 0; i < totalRenderables; i++ )
	{
		if( m_renderables[i] == &removeMe )
		{
			// Swap it with the last one
			std::swap( m_renderables[i], m_renderables[ totalRenderables - 1 ] );

			// pop back
			m_renderables.pop_back();

			// return..
			return;
		}
	}
}
