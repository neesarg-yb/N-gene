#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
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

class Material;

typedef std::vector< GameObject* > GameObjectList;

class Level
{
public:
	 Level( std::string definitionName );
	~Level();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

public:
	LevelDefinition const		&m_definition;

private:
	// Rendering Specific
	static OrbitCamera*			s_camera;
	static Scene*				s_levelScene;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.7f );
	ForwardRenderingPath*		m_renderingPath				= nullptr;
	
	GameObjectList				m_allGameObjects;
	Robot*						m_playerRobot				= nullptr;
	Tower*						m_tower						= nullptr;

	// Level Specific
	double						m_timeSinceStartOfTheBattle	= 0;
	Renderable*					m_sphere					= nullptr;
	Material*					m_sphereMaterial			= nullptr;
	Mesh*						m_sphereMesh				= nullptr;

	// PickBuffer
	PickBuffer					m_pickBuffer;

public:
	// Lights
	static std::vector< Light* > s_lightSources;
	
	static void	AddNewPointLightToCamareaPosition( Rgba lightColor );

private:
	double	GetTimeSinceBattleStarted() const;
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );
	
};