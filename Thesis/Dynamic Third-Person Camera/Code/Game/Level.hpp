#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/PickBuffer.hpp"
#include "Game/GameObject.hpp"
#include "Game/World/Tower.hpp"
#include "Game/World/Block.hpp"
#include "Game/World/Robot.hpp"
#include "Game/World/LevelDefinition.hpp"

typedef std::vector< GameObject* > GameObjectList;

class Material;

class Level
{
public:
	 Level( char const * levelName );
	~Level();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;
	
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