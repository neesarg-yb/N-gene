#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/GameObject.hpp"
#include "Game/World/Block.hpp"

class Material;

typedef std::vector< GameObject* > GameObjectList;

class Level
{
public:
	Level();
	~Level();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

private:
	// Rendering Specific
	static OrbitCamera*			s_camera;
	static Scene*				s_levelScene;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.7f );
	ForwardRenderingPath*		m_renderingPath				= nullptr;
	
	GameObjectList				m_allGameObjects;

	// Battle Specific
	double						m_timeSinceStartOfTheBattle	= 0;
	Renderable*					m_sphere						= nullptr;
	Material*					m_sphereMaterial				= nullptr;
	Mesh*						m_sphereMesh					= nullptr;

public:
	// Lights
	static std::vector< Light* > s_lightSources;
	
	static void	AddNewPointLightToCamareaPosition( Rgba lightColor );

private:
	double	GetTimeSinceBattleStarted() const;
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );
	
};