#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Tank.hpp"

class Material;

typedef std::vector< GameObject* > GameObjectList;

class Battle
{
public:
	 Battle();
	~Battle();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

private:
	// Rendering Specific
	static Camera*				 s_camera;
	static Scene*				 s_battleScene;
	Vector4						 m_ambientLight					= Vector4( 1.f, 1.f, 1.f, 0.2f );
	ForwardRenderingPath*		 m_renderingPath				= nullptr;

	// Battle Specific
	double						 m_timeSinceStartOfTheBattle	= 0;
	Renderable*					 m_sphere						= nullptr;
	Material*					 m_sphereMaterial				= nullptr;
	Mesh*						 m_sphereMesh					= nullptr;

	GameObjectList				 m_allGameObjects;


public:
	// Lights
	static std::vector< Light* > s_lightSources;
	
	static void	AddNewPointLightToCamareaPosition( Rgba lightColor );

private:
	double	GetTimeSinceBattleStarted() const;
	void	RotateTheCameraAccordingToPlayerInput	( float deltaSeconds );
	void	ChnageLightAsPerInput					( float deltaSeconds );
	
};