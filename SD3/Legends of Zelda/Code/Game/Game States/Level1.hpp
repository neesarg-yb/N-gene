#pragma once
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/Abstract Classes/GameObject.hpp"

typedef std::vector< GameObject* > GameObjectList;

class Level1 : public GameState
{
public:
	 Level1();
	~Level1();

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;

private:
	// Rendering Specific
	OrbitCamera*				m_camera					= nullptr;
	Scene*						m_levelScene				= nullptr;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.5f );
	ForwardRenderingPath*		m_renderingPath				= nullptr;

public:
	std::string					m_levelName;

	// Lights
	std::vector< Light* >		m_lightSources;
	GameObjectList				m_allGameObjects;

private:
	// Local
	double						m_timeSinceStartOfTheBattle	= 0;
};